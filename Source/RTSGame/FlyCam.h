#pragma once

#include <vector>
using namespace std;

#include "Types.h"
#include "GameFramework/Pawn.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
#include "FlyCam.generated.h"

class AMyHUD;
class APlayerControl;
struct FWidgetData;
class AGameObject;
struct GraphNode;
struct Edge;
class Pathfinder;
class UDialogBox;
class UMenu;
class UTipsBox;

UCLASS()
class RTSGAME_API AFlyCam : public APawn
{
	GENERATED_BODY()
public:
  AActor *floor;
  AGameObject *ghost;
  Pathfinder *pathfinder;
  UCameraComponent *camera;

  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Pathfinding )  int32 Rows;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Pathfinding )  int32 Cols;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Pathfinding )  bool VizGrid;
  
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Sounds )  USoundBase* bkgMusic;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Sounds )  USoundBase* buildingPlaced;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Sounds )  USoundAttenuation* soundAttenuationSettings;

  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Colors )  UMaterial* White;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Colors )  UMaterial* Red;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Colors )  UMaterial* Green;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = Colors )  UMaterial* Yellow;

  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = UIBoxes )  UClass* DialogBoxBlueprint;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = UIBoxes )  UClass* MenuBlueprint;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = UIBoxes )  UClass* TipsBoxBlueprint;
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = UIBoxes )  TArray<FString> Tips;

  // the name of the title level
  UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = UOptions )  FName LevelMap;

  UAudioComponent* music;
  float sfxVolume; // volume at which new SFX are played
  UDialogBox *dialogBox;
  UMenu *menu;
  UTipsBox *tipsBox;
  int tipNumber;
  
  // this flag indicates if the level has been initialized yet
  bool setupLevel;
  float movementSpeed;
  UFloatingPawnMovement *MovementComponent;
  UInputComponent* InputComponent;
  vector<AActor*> viz;
  
  // Sets default values for this pawn's properties
	AFlyCam( const FObjectInitializer& PCIP );
  
  // Called to bind functionality to input
  virtual void SetupPlayerInputComponent( UInputComponent* InputComponent ) override;
  
  // Start to load the map in levelName
  void LoadLevel( FName levelName );
  // Once the level is loaded, 
  void OnLevelLoaded();
  // Initializes the level 
  void InitializePathfinding();
  void UnloadLevel();
  
  void SetCameraPosition( FVector2D perc );
  void NextTip();
  void DisplayMenu();
  FHitResult LOS( FVector p, FVector q, TArray<AActor*> ignoredActors );
  static void SetColor( AActor* a, UMaterial* mat );
  // Series of points to visualize
  void Visualize( FVector& v, UMaterial* color );
  void Visualize( vector<FVector>& v );
  AActor* MakeSphere( FVector center, float radius, UMaterial* color );
  AActor* MakeCube( FVector center, float radius, UMaterial* color );
  AActor* MakeLine( FVector a, FVector b, UMaterial* color );
  void RetrievePointers();
  void debug( int slot, FColor color, FString mess );
  void setGhost( Types ut );
  
  FVector2D getMousePos();
  FHitResult getHitGeometry();
  vector<FHitResult> getAllHitGeometry();
  FVector getHitFloor(FVector eye, FVector look);
  FVector getHitFloor();
  bool intersectsAny( AActor* actor );
  bool intersectsAny( AActor* actor, vector<AActor*>& except );
  bool intersectsAnyOfType( AActor* actor, vector<Types>& types );
  
  void FindFloor();
  void MouseLeftDown();
  void MouseLeftUp();
  void MouseRightDown();
  void MouseRightUp();
  void MouseMoved();
  void MouseMovedX( float amount );
  void MouseMovedY( float amount );
  
  void MoveCameraZUp( float amount );
  void MoveCameraZDown( float amount );
  void MoveForward( float amount );
  void MoveBack( float amount );
  void MoveLeft( float amount );
  void MoveRight( float amount );
  
	virtual void BeginPlay() override;
	virtual void Tick( float t ) override;
	
};
