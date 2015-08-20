#include "Wryv.h"
#include "BuildQueue.h"
#include "Clock.h"

FLinearColor BuildQueue::ClockColor( 0,0,0,0.15 );

BuildQueue::BuildQueue( FString name, FVector2D entrySize ) : 
  StackPanel( name, StackPanel::StackPanelTexture ), EntrySize( entrySize )
{
  Pad = FVector2D( 4, 4 );
  Align = BottomCenter;
  Selected = 0;
  needsRefresh = 0;
  Size = FVector2D(0,0);
}

void BuildQueue::Clear()
{
  StackPanel::Clear();
  clocks.clear(); // clear extra references
}

void BuildQueue::Set( AGameObject* go )
{
  needsRefresh = 0;
  Selected = go;
  // When you call SET, you clear all the old buildcounters.
  // Clocks inside the buildcounter are not cached.
  Clear();
  recomputeSizeToContainChildren();
  if( !go )  return;

  // Things that are spawning each have a clock.
  for( int i = 0; i < go->BuildQueueCounters.size(); i++ )
  {
    CooldownCounter counter = go->BuildQueueCounters[i];
    Clock* clock = new Clock( FS( "Cooldown %d", i ),
      EntrySize, Game->GetPortrait( counter.Type ), ClockColor );
    StackRight( clock, VCenter );
    clocks.push_back( clock );
    
    // Register a function to remove the widget from this queue when clicked.
    clock->OnMouseDownLeft = [this,go,clock,i](FVector2D mouse)
    {
      LOG( "Cancelled item [%d] belonging to %s", i, *go->Stats.Name );
      removeIndex( go->BuildQueueCounters, i );
      needsRefresh = 1; // reset the buildQueue's contents.
      return Consumed;
    };
  }
}

void BuildQueue::Refresh()
{
  needsRefresh = 1;
}

void BuildQueue::Move( float t )
{
  if( needsRefresh )
  {
    Set( Selected );
  }

  if( Selected )
  {
    // refresh the counts
    for( int i = 0; i < Selected->BuildQueueCounters.size(); i++ )
    {
      if( i >= clocks.size() )
      {
        error( FS( "Index %d OOB the clocks", i ) );
        Set( Selected ); // reset the clocks
        break;
      }
      else
      {
        clocks[i]->Set( Selected->BuildQueueCounters[i].Percent() );
      }
    }
  }
}