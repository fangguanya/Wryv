#pragma once

#include <deque>
#include <map>
#include <set>
#include <vector>
#include <Array.h>
#include <List.h>
using namespace std;

#include "GameFramework/Actor.h"

#include "Command.h"
#include "CooldownCounter.h"
#include "GlobalFunctions.h"
#include "SoundEffect.h"
#include "UnitsData.h"

#include "GameObject.generated.h"

class AItem;
class AGameObject;
class AResource;
class AShape;
struct Team;

class UAction;
class UBuildAction;
class UBuildInProgress;
class UCastSpellAction;
class UItemAction;
class UResearch;
class UTrainingAction;

UCLASS()
class WRYV_API AGameObject : public AActor
{
  GENERATED_UCLASS_BODY()
  const static float WaypointAngleTolerance; // 
  const static float WaypointReachedToleranceDistance; // The distance to consider waypoint as "reached"
  static AGameObject* Nothing;
  static float CapDeadTime; // Dead units get cleaned up after this many seconds (time given for dead anim to play out)
  
  // Stats.
  int64 ID; // Unique Identity of the object.
  Team *team;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnitProperties)  FUnitsDataRow BaseStats;
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitProperties)  FUnitsDataRow Stats;
  // The stats applied due to research bonuses.
  vector<FUnitsDataRow> ResearchBonusStats;
  // Level of the object.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnitData) int32 Level;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnitData) int32 Kills;

  // The amount that this object multiplies incoming repulsion forces by.
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnitProperties)  float RepelMultiplier;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnitProperties)  USceneComponent* DummyRoot;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnitProperties)  UCapsuleComponent* hitBounds;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UnitProperties)  USphereComponent* repulsionBounds;

  vector< PowerUpTimeOut > BonusTraits;
  float Hp;             // Current Hp. float, so heal/dmg can be continuous (fractions of Hp)
  float Mana;           // Current Mana.
  UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = UnitProperties)  bool Dead;            // Whether unit is dead or not.
  float DeadTime, MaxDeadTime; // how long the object has been dead for
  FLinearColor vizColor;
  float vizSize;
  bool Recovering;

  // 
  // COOLDOWNS: A list of these for each of our capabilities in Stats.
  vector< UAction* > CountersAbility;    // All classes
  // List of objects that are currently being built by this object.
  vector< UBuildAction* > CountersBuildings; // Todo: Peasant only
  vector< UBuildInProgress* > CountersBuildQueue; // Todo: Peasant only
  vector< UItemAction* > CountersItems;      // Todo: Units
  vector< UResearch* > CountersResearch;   // Todo: For Buildings only
  vector< UTrainingAction* > CountersTraining;   // Todo: For Buildings only
  // Items unit is holding in-play.

  int64 LastBuildingID; // The ID of the last building we proposed to be placed.
  
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Sounds )  TArray<FSoundEffect> Greets;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Sounds )  TArray<FSoundEffect> Oks;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Sounds )  TArray<FSoundEffect> Attacks;

  // 
  // Movement & Attack.
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitProperties)  FVector Pos;
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitProperties)  FVector Dir;
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitProperties)  float Speed;
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitProperties)  FVector Vel;
  FVector Dest;
  vector<FVector> Waypoints;
  vector<AShape*> NavFlags; // Debug info for navigation flags

  // Series of commands.
  deque<Command> commands;
  bool IsReadyToRunNextCommand; // This Flag is an instruction to execute the next command.
  // Set when idling, OR when manually asked to change command to a new one.
  bool AttackReady; // When this flag is set, the unit will engage any enemy units.
  // Peasants rarely have this flag set, while most combat units have it set (unless they are
  // responding to a GotoGroundPosition() command)
  bool HoldingGround; // Do not move to engage nearby enemy units.
  // When this flag is false, AttackTarget is set to the nearest unit in SightRange.

  Command& GetCurrentCommand(){ return commands.front(); }

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitProperties)  AGameObject* FollowTarget;
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = UnitProperties)  AGameObject* AttackTarget;
  // Cached collections of followers & attackers
  vector<AGameObject*> Followers, Attackers, RepulsionOverlaps;
  FVector AttackTargetOffset;  // Ground position of spell attacks

  // 
  // UE4 & Utility
  template <typename T> vector<T*> GetComponentsByType() {
    return ::GetComponentsByType<T>( this );
  }
  virtual void PostInitializeComponents();
  virtual void BeginPlay() override;
  virtual void OnMapLoaded();
  
  // Net
  // Hashes the object (checking for network state desync)
  float Hash();
  
  // 
  // Scenegraph management.
  AGameObject* SetParent( AGameObject* newParent );
  AGameObject* AddChild( AGameObject* newChild );
  bool isParentOf( AGameObject* go );
  bool isChildOf( AGameObject* parent );
  template <typename T> T* MakeChild( TSubclassOf<AGameObject> ClassType )
  {
    T* child = Game->Make<T>( ClassType, team );
    AddChild( child );
    return child;
  }
  void SetSize( FVector size );

  // 
  // Gameplay.
  FVector GetTip();
  FVector GetCentroid();
  float GetHeight();
  float centroidDistance( AGameObject *go );
  float outerDistance( AGameObject *go );
  UFUNCTION(BlueprintCallable, Category = Fighting)  bool isAttackTargetWithinRange();
  UFUNCTION(BlueprintCallable, Category = Fighting)  bool isFollowTargetWithinRange();
  UFUNCTION(BlueprintCallable, Category = Fighting)  float HpFraction();
  UFUNCTION(BlueprintCallable, Category = Fighting)  float SpeedPercent();
  // Pass thru to stats structure property
  UFUNCTION(BlueprintCallable, Category = Fighting)  float AttackSpeedMultiplier() { return Stats.AttackSpeedMultiplier; }
  UFUNCTION(BlueprintCallable, Category = Fighting)  bool hasAttackTarget() { return AttackTarget != 0; }
  UFUNCTION(BlueprintCallable, Category = Fighting)  bool hasFollowTarget() { return FollowTarget != 0; }
  // Called by blueprints (AttackAnimation) when attack is launched (for ranged weapons)
  // or strikes (for melee weapons).
  UFUNCTION(BlueprintCallable, Category = Fighting)  virtual void AttackCycle();
  inline float DamageRoll() { return Stats.BaseAttackDamage + randFloat( Stats.BonusAttackDamage ); }
  void Shoot();
  virtual void ReceiveAttack( AGameObject* from );

  void UpdateStats( float t );

  // Use* functions use specific indexes from Abilities,
  // Builds, Training, Researches, or Items slots.
  bool UseAbility( int index );
  bool UseBuild( int index );
  bool UseTrain( int index );
  bool UseResearch( int index );
  bool UseItem( int index );
  
  // 
  // Movement functions.
  void SetRot( const FRotator & ro );
  bool Reached( FVector& v, float dist );
  void CheckWaypoint();
  void SetPosition( FVector v );
  void FlushPosition();
  FVector Repel( AGameObject* go );

  UFUNCTION(BlueprintNativeEvent, Category = Collision)
  void OnHitContactBegin( AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult );
  UFUNCTION(BlueprintNativeEvent, Category = Collision)
  void OnHitContactEnd( AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );
  
  UFUNCTION(BlueprintNativeEvent, Category = Collision)
  void OnRepulsionContactBegin( AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult );
  UFUNCTION(BlueprintNativeEvent, Category = Collision)
  void OnRepulsionContactEnd( AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex );
  
  void AddRepulsionForcesFromOverlappedUnits();
  // Walks towards Dest.
  void Walk( float t );
  // Points Actor to face particular 3-space point
  void Face( FVector point );
  // time-stepped attack of current target (if any)
  void MoveWithinDistanceOf( AGameObject* target, float fallbackDistance );
  void DisplayWaypoints();
  void exec( const Command& cmd );
  // Base Movement function. Called each frame.
  //   * Fixed time step
  //   * Predictable call order
  // This function is provided INSTEAD of ::Tick(), which has variable timestep value 
  // and unpredicatable call order.
  virtual void Move( float t );
  virtual bool Idling();
  virtual void ai( float t );
  // The hit volumes overlapped
  virtual void Hit( AGameObject* other );
  float Height();
  float Radius();

  //
  // COMMAND
  // Sets a new ground destination, dropping Follow/Attack targets.
  virtual void GoToGroundPosition( FVector groundPos );
  // Goes towards ground position, while attacking any units that are within SightRange units of it.
  virtual void AttackGroundPosition( FVector groundPos );
  // Sets a particular object as a Target (due to generic right click).
  // Resolves if target is ally (follow) or opponent (attack).
  virtual void Target( AGameObject* target );
  void Follow( AGameObject* go );
  void Attack( AGameObject* go );
  
  // Moves towards d using pathfinder, WITHOUT losing Follow/Attack targets.
  void SetDestination( FVector d );
  // Stops motion, WITHOUT losing Follow/Attack targets.
  void StopMoving();
  // Stop command, which aborts current command, and cancels all queued commands.
  void Stop();
  
  // 
  // Unit relationship functions
  bool isAllyTo( AGameObject* go );
  bool isEnemyTo( AGameObject* go );

  // Drop my attack & follow targets
  virtual void DropTargets();
  void LoseFollower( AGameObject* formerFollower );
  void LoseAttacker( AGameObject* formerAttacker );
  // Causes all attackers and followers to stop attacking and following this unit.
  void Stealth();

  // 
  // AI
  AGameObject* GetClosestEnemyUnit();
  map<float, AGameObject*> FindEnemyUnitsInSightRange();
  // Searches for an object of specific type on my TEAM
  AGameObject* GetClosestObjectOfType( TSubclassOf<AGameObject> ClassType );
	
  // 
  // Utility
  void Flags( vector<FVector> points, FLinearColor color );
  void Viz( FVector pt );
  void OnSelected();
  void SetMaterialColors( FName parameterName, FLinearColor color );
  void SetTeam( Team* newTeam );
  void PlaySound( USoundBase* sound ){ UGameplayStatics::SpawnSoundAttached( sound, GetRootComponent() ); }
  void SetMaterial( UMaterialInterface* mat );
  void SetColor( FLinearColor color );
  
  // Returns true if this object is a subclass of any of the types listed.
  // Eg this is a Projectile, then { ASpell::StaticClass(), AProjectile::StaticClass() }
  // would return true.
  template <typename T>
  bool IsAny( set< TSubclassOf< T > > types )
  {
    static_assert( is_base_of< AGameObject, T >::value, "Populate<T>: T must derive from AGameObject" );

    for( TSubclassOf<AGameObject> uc : types )
    {
      if( this->IsA( uc ) )
      {
        info( FS( "%s IsA( %s )", *Stats.Name, *(*uc)->GetName() ) );
        return 1;
      }
    }
    return 0;
  }

  // Shouldn't have to reflect unit type often, but we use these
  // to select a building for example from the team's `units` collection.
  // Another way to do this is orthogonalize collections [buildings, units.. etc]
  virtual void Die();
  UFUNCTION(BlueprintCallable, Category = Fighting) 
  void Cleanup();
  virtual void BeginDestroy() override;

  UFUNCTION(BlueprintCallable, Category = Display) FString ToString();
  UFUNCTION(BlueprintCallable, Category = Display) FString FollowersToString();
  
};
