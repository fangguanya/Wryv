#pragma once

#include "Action.h"
#include "InProgressBuilding.generated.h"

class ABuilding;

// A UInProgressBuilding is a counter for a building that is being built.

UCLASS( meta=(ShortTooltip="Something that is being built indicated") )
class WRYV_API UInProgressBuilding : public UAction
{
  GENERATED_UCLASS_BODY()
public:
  // The building the in-progress counter is attached to.
  ABuilding* Building;
  APeasant* Peasant;

  // Cancels the in-progress building.
  void SetBuilding(ABuilding* building);

  virtual void Cancel();
  virtual void OnComplete();
  virtual void Step( float t );
};