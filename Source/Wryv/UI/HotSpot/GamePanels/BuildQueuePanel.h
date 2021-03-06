#pragma once

#include "GameObjects/GameObject.h"
#include "UI/HotSpot/Panels/StackPanel.h"
#include "UE4/WryvGameInstance.h"

// This is the object that describes the units we are building
class BuildQueuePanel : public StackPanel
{
public:
  FVector2D EntrySize;
  
  // A BuildQueuePanel contains a list of things that are being built
  BuildQueuePanel( FString name, FVector2D entrySize, int maxTrains );
  void Set( vector<AGameObject*> objects );
  virtual void render( FVector2D offset ) {
    StackPanel::render( offset ) ;
  }
};
