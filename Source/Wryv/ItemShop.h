#pragma once

#include "Array.h"
#include "Building.h"
#include "ItemShop.generated.h"

class AUnit;
class AItem;

UCLASS()
class WRYV_API AItemShop : public ABuilding
{
	GENERATED_UCLASS_BODY()
public:
  // The nearest ItemShop patron.
  AUnit* patron;

  // A listing of the items contained inside the shop
  TArray< TSubclassOf< AItem > > Inventory;

  virtual void BeginPlay() override;

};
