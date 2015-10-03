#pragma once

#include "Action.h"
#include "TrainingAction.generated.h"

class ABuilding;
class AGameObject;
class AUnit;

// Clicking on one of these kicks off the training of a unit.
// The unit does NOT get created until the progress completes.
// 
UCLASS( BlueprintType, Blueprintable, meta=(ShortTooltip="A training action") )
class WRYV_API UTrainingAction : public UAction
{
  GENERATED_UCLASS_BODY()
public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
  TSubclassOf< AUnit > UnitType;
  
  // The building that is training the unit.
  ABuilding* Building;

  // When you click this button, it kicks off building a unit of UnitType.
  virtual void Click(ABuilding* building);
  virtual void OnRefresh();
  virtual void OnComplete();
  
};