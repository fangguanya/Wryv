#include "Wryv.h"

#include "GameObjects/Buildings/Building.h"
#include "UI/UICommand/Command/UIBuildActionCommand.h"
#include "UI/HotSpot/GamePanels/BuildQueuePanel.h"
#include "UI/HotSpot/Elements/Clock.h"
#include "Util/GlobalFunctions.h"
#include "GameObjects/Units/Peasant.h"
#include "UI/HotSpot/Panels/SlotPanel.h"
#include "GameObjects/Units/Unit.h"

#include "UI/UICommand/Counters/UIInProgressBuildingCounter.h"
#include "UI/UICommand/Counters/UIInProgressUnitCounter.h"
#include "UI/UICommand/Counters/UIInProgressResearchCounter.h"
#include "UI/UICommand/Command/UIResearchCommand.h"
#include "UI/UICommand/Command/UITrainingActionCommand.h"

BuildQueuePanel::BuildQueuePanel( FString name, FVector2D entrySize, int maxTrains ) : 
  StackPanel( name, StackPanel::StackPanelTexture ), EntrySize( entrySize )
{
  Pad = FVector2D(4,4);
  Align = BottomCenter;
  Size = FVector2D(0,0);

  // Initialize with 10 clocks (max trains)
  for( int i = 0; i < maxTrains; i++ )
  {
    Clock *clock = new Clock( FS( "Units in progress %d", i ), EntrySize, NoTextureTexture, 
      FLinearColor::White, Alignment::CenterCenter );
    StackRight( clock, VCenter );
  }
}

void BuildQueuePanel::Set( vector<AGameObject*> objects )
{
  // When you call SET, you clear all the old buildcounters.
  // Clocks inside the buildcounter are not cached.
  HideChildren(); // Assume hidden

  for( int i = 0; i < objects.size(); i++ )
  {
    AGameObject* go = objects[i];
    if( ABuilding* building = Cast<ABuilding>( go ) )
    {
      // How many clocks are needed in the build queue?
      int numClocks = building->CountersUnitsInProgress.Num() + building->CountersResearchInProgress.Num();
      if( !numClocks )  return; // No clocks to show.
      
      // Populate with the Units in progress first if any.
      for( int i = 0; i < building->CountersUnitsInProgress.Num(); i++ )
      {
        UUIInProgressUnit* inProgress = building->CountersUnitsInProgress[i];
        Clock* clock = (Clock*)GetChild(i);
        if( clock ) inProgress->PopulateClock( clock, i );
        else error( "Out of clocks" );
      }

      // Populate with any ResearchInProgress.
      int j = building->CountersUnitsInProgress.Num();
      for( int i = 0; i < building->CountersResearchInProgress.Num(); i++ )
      {
        UUIInProgressResearch* research = building->CountersResearchInProgress[i];
        Clock* clock = (Clock*)GetChild(i + j);
        if( clock )  research->PopulateClock( clock, i + j );
        else  error( "Out of clocks" );
      }
    }
    else if( APeasant* peasant = Cast<APeasant>( go ) ) // Throw in also any peasant's buildqueue
    {
      for( int i = 0; i < peasant->CountersBuildingsQueue.Num(); i++ )
      {
        UUIInProgressBuilding* buildingInProgress = peasant->CountersBuildingsQueue[i];
        Clock* clock = (Clock*)GetChild(i);
        if( clock )  buildingInProgress->PopulateClock( clock, i );
        else  error( "Out of clocks" );
      }
    }
  }
}

