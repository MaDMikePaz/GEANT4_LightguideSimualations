
//
/// \file B4aEventAction.cc
/// \brief Implementation of the B4aEventAction class

#include "B4aEventAction.hh"
#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

G4double bins = 0;
G4double bins1 = 0;
G4double T1 = 0;
G4double Event_number= 1;
std::ofstream myfile;

B4aEventAction::B4aEventAction()
    : G4UserEventAction()

{
  //  myfile.open("EventSummary.csv",std::ofstream::out);

    //myfile << "EventID"                     << ","
        //   << "FirstPhotonTime_mySiPM1/ns"  << ","
        //   << "FirstPhotonTime_mySiPM2/ns"  << ","
        //   << "Nphotons_mySiPM1"            << ","
       //    << "Nphotons_mySiPM2"            << ","
      //     << std::endl;

    //myfile.close();
}




B4aEventAction::~B4aEventAction()
{}
void B4aEventAction::BeginOfEventAction(const G4Event* evt/*event*/)
{
    bins = 0;
    bins1 = 0;
    Event_number++;
    for (int i=0; i<NphotonsMax; i++)
    {
        PhotonHitUpperFace[i] = FALSE;
    }
    eventID = evt->GetEventID();
    // detection time of event in the two "SiPMs"
    // initialize as -1 so that in the file we know if we detected an event or not
    FirstPhotonTime_mySiPM1 = -1;
    FirstPhotonTime_mySiPM2 = -1;

    // total number of photons that were absorbed in the "SiPMs"
    Nphotons_mySiPM1 = Nphotons_mySiPM2 = 0;



}
void B4aEventAction::EndOfEventAction(const G4Event* event)
{

    if(bins > 0)
    {
         std::ofstream myfile;
         myfile.open("Photo_e_20mm_LG_0_20.csv",std::ofstream::out|std::ofstream::app);
         myfile << bins << std::endl;
        // G4cout << bins << " , " ;
         myfile.close();
    // bins = 0;
    }
}

























































































































































































































































