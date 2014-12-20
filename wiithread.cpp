#include "wiithread.h"

#include <wiicpp.h>

void WiiThread::run() Q_DECL_OVERRIDE {
    QString result;

    CWii wii; // Defaults to 4 remotes
    std::vector<CWiimote>::iterator i;
    int reloadWiimotes = 0;
    int numFound;
    int index;

    cout << "Searching for wiimotes... Turn them on!" << endl;

    //Find the wiimote
    numFound = wii.Find(5);

    // Search for up to five seconds;

    //cout << "Found " << numFound << " wiimotes" << endl;
    //cout << "Connecting to wiimotes..." << endl;

    // Connect to the wiimote
    std::vector<CWiimote>& wiimotes = wii.Connect();

    //cout << "Connected to " << (unsigned int)wiimotes.size() << " wiimotes" << endl;

    emit connected();

    // Setup the wiimotes
    for(index = 0, i = wiimotes.begin(); i != wiimotes.end(); ++i, ++index)
    {
        // Use a reference to make working with the iterator handy.
        CWiimote & wiimote = *i;

        //Rumble for 0.2 seconds as a connection ack
        wiimote.SetRumbleMode(CWiimote::ON);
        usleep(200000);
        wiimote.SetRumbleMode(CWiimote::OFF);
    }

    do
    {
        if(reloadWiimotes)
        {
            // Regenerate the list of wiimotes
            wiimotes = wii.GetWiimotes();
            reloadWiimotes = 0;
        }

        //Poll the wiimotes to get the status like pitch or roll
        if(wii.Poll())
        {
            for(i = wiimotes.begin(); i != wiimotes.end(); ++i)
            {
                // Use a reference to make working with the iterator handy.
                CWiimote & wiimote = *i;
                switch(wiimote.GetEvent())
                {

                    case CWiimote::EVENT_EVENT:
                        if(wiimote.ExpansionDevice.GetType() == wiimote.ExpansionDevice.TYPE_BALANCE_BOARD)
                        {
                           CBalanceBoard &bb = wiimote.ExpansionDevice.BalanceBoard;
                           float total, topLeft, topRight, bottomLeft, bottomRight;
                           bb.WeightSensor.GetWeight(total, topLeft, topRight, bottomLeft, bottomRight);
                           emit gotEvent(total, topLeft, topRight, bottomLeft, bottomRight);
                       }
                        break;

                    case CWiimote::EVENT_BALANCE_BOARD_INSERTED:
                                cout << "Balance Board connected.\n"  << endl;
                                break;

                   case CWiimote::EVENT_BALANCE_BOARD_REMOVED:
                               cout << "Balance Board disconnected.\n"  << endl;
                                break;

                    default:
                        break;
                }
            }
        }

    } while(wiimotes.size()); // Go so long as there are wiimotes left to poll

    cout << "End of game.\n"  << endl;
    emit disconnected();
}
