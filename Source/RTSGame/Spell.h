#pragma once

#include "GameObject.h"
#include "WidgetData.h"
#include "GameFramework/Actor.h"
#include "Spell.generated.h"

UCLASS()
class RTSGAME_API ASpell : public AGameObject
{
  GENERATED_UCLASS_BODY()
public:
  AGameObject* caster;
  bool init;

  // Which object type is spawned on contact (usually a Spell)
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=UnitProperties)
	TEnumAsByte<Types> OnContact;

  void Init();

  UFUNCTION(BlueprintNativeEvent, Category = Collision)
  void ProxSpell( AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult & SweepResult );

  // Called when the game starts or when spawned
  virtual void BeginPlay() override;
  // Called every frame
  virtual void Tick( float t ) override;
  virtual void ai( float t );
};
