#include "Wryv.h"
#include "ActionsPanel.h"
#include "Clock.h"
#include "WryvGameInstance.h"
#include "GameObject.h"

ActionsPanel::ActionsPanel( FString name, FVector2D entrySize ) : HotSpot( name )
{
  Align = TopCenter;

  abilities = new AbilitiesPanel( this, SlotPalette::SlotPaletteTexture, 2, 3,
    entrySize, FVector2D(8,8) );
  Add( abilities );

  buildPanel = new BuildPanel( this, SlotPalette::SlotPaletteTexture, 2, 3,
    entrySize, FVector2D(8,8) );
  buildPanel->Hide();
  Add( buildPanel );

  recomputeSizeToContainChildren();
}

void ActionsPanel::ShowAbilitiesPanel()
{
  abilities->Show();
  buildPanel->Hide();
}

void ActionsPanel::ShowBuildingsPanel()
{
  buildPanel->Show();
  abilities->Hide();
}

void ActionsPanel::Set( AGameObject* go )
{
  abilities->Set( go );
  buildPanel->Set( go );
}



AbilitiesPanel::AbilitiesPanel( ActionsPanel* iActions, UTexture* bkg, int rows, int cols, FVector2D entrySize, FVector2D pad ) : 
  SlotPalette( "abilities panel", bkg, rows, cols, entrySize, pad )
{
  Align = TopCenter;
  actions = iActions;
  if( GetNumSlots() )
  {
    buildButton = GetSlot( GetNumSlots() - 1 );
    buildButton->Tex = BuildButtonTexture;
    buildButton->OnMouseDownLeft = [this]( FVector2D mouse ) {
      actions->ShowAbilitiesPanel();
      return Consumed;
    };
  }
}

void AbilitiesPanel::Set( AGameObject *go )
{
  // start by hiding all abilities
  HideChildren();
  if( !go ) return;

  vector<Clock*> abilitiesClocks = Populate( go->Stats.Abilities );

  for( int i = 0; i < abilitiesClocks.size(); i++ )
  {
    Clock* clock = abilitiesClocks[ i ];
    // Attach button with invokation of i'th ability
    clock->OnMouseDownLeft = [go,i]( FVector2D mouse ) {
      // Invoke I'th action of the object
      go->UseAbility( i );
      return Consumed;
    };
    
  }
  
  buildButton->Hide();
  if( go->isPeasant() ) buildButton->Show();
}



BuildPanel::BuildPanel( ActionsPanel* iActions, UTexture* bkg, int rows, int cols, FVector2D entrySize, FVector2D pad ) : 
  SlotPalette( "BuildPanel", bkg, rows, cols, entrySize, pad )
{
  Align = TopCenter;
  actions = iActions;
}

void BuildPanel::Set( AGameObject *go )
{
  HideChildren(); // Hide all buttons
  if( ! go )  return;

  // the rest of the abilities can be set to null
  for( int i = go->Stats.Abilities.Num(); i < GetNumSlots(); i++ )
  {
    // Turn off the function object, jsut in case
    GetSlot(i)->OnMouseDownLeft = function<EventCode (FVector2D mouse)>(); // null the callback
    // hide slot 
    GetSlot(i)->Hide();
  }
  
}
