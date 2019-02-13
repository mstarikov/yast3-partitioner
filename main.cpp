#include "string"
#include "list"
#include "sstream"
#include "fstream"
#include "cstdio"

#include "nlohmann/json.hpp"

#include "YUI.h"
#include "YApplication.h"
#include "YWidgetFactory.h"
#include "YDialog.h"
#include "YFrame.h"

#include "YLabel.h"
#include "YLayoutBox.h"
#include "YEvent.h"
#include "YWidget.h"
#include "YAlignment.h"
#include "YSquash.h"
#include "YCheckBoxFrame.h"
#include "YTree.h"

#include "YTable.h"
#include "YTableHeader.h"

using namespace std;
using json = nlohmann::json;

#define LAYOUT_HELPER_CONVENIENCE( AL ) \
inline YWidget * at##AL ( YWidget * w ) { return YUI::widgetFactory()->create##AL( w ); }
LAYOUT_HELPER_CONVENIENCE( Left );
LAYOUT_HELPER_CONVENIENCE( Right );
LAYOUT_HELPER_CONVENIENCE( Top );
LAYOUT_HELPER_CONVENIENCE( Bottom );
LAYOUT_HELPER_CONVENIENCE( HCenter );
LAYOUT_HELPER_CONVENIENCE( VCenter );
LAYOUT_HELPER_CONVENIENCE( HVCenter );

inline YWidget * labeledFrameBox( YWidget * parent, const std::string & label, int hweight = 0, bool squashed = true )
{
  YWidget * frame	= YUI::widgetFactory()->createFrame( parent, label );
  if ( hweight )
    frame->setWeight( YD_HORIZ, hweight );
  frame = YUI::widgetFactory()->createHVCenter( frame );
  if ( squashed )
    frame = YUI::widgetFactory()->createHVSquash( frame );
  frame	= YUI::widgetFactory()->createVBox( frame );
  return frame;
}

inline YWidget * unsquashedLabeledFrameBox( YWidget * parent, const std::string & label, int hweight = 0 )
{ return labeledFrameBox( parent, label, hweight, false ); }


void runAnsible(string arguments)
{
    if (remove("./ansible_facts.json") != 0)
        perror("Error deleting file" );
    else
        puts("File successfully deleted");

    ofstream ansible_facts_document("./ansible_facts.json");
    string program = "/usr/bin/ansible " + arguments;
    const char *command = program.c_str();

    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    stream = popen(command, "r");
    if (stream)
    {
        while (!feof(stream))
        {
            if (fgets(buffer, max_buffer, stream) != NULL)
            {
                if( strstr(buffer,"SUCCESS => {"))
                {
                    ansible_facts_document << "{\n";
                }
                else
                {
                    ansible_facts_document << buffer;
                }

            }
        }
        pclose(stream);
    }
    ansible_facts_document.close();
}

json parseAnsible()
{
    ifstream facts_file("./ansible_facts.json");
    json facts_object = json::parse(facts_file)["ansible_facts"]["ansible_devices"];
    return facts_object;
}

int main( int argc, char **argv )
{

    //runAnsible("localhost -m setup | sed -e 's/localhost | SUCCESS =>/''/g' > ./ansible_facts.json");
    //string arguments = "localhost -m setup";
    //runAnsible(arguments);
    json ansible_devices = parseAnsible();
    //YUILog::enableDebugLogging( true );

    // auto f-keys assigned to Buttoms with that label
    // see YButtonBox for button roles.
    YUI::app()->setDefaultFunctionKey( "Help",  1 );
    YUI::app()->setDefaultFunctionKey( "Cancel",  9 );
    YUI::app()->setDefaultFunctionKey( "Quit", 10 );
    YUI::app()->setDefaultFunctionKey( "Ok", 10 );

    // layout dialog:
    YDialog * dialog = YUI::widgetFactory()->createMainDialog();
    YLayoutBox * vbox = YUI::widgetFactory()->createVBox( dialog );
    YWidget * hbox = YUI::widgetFactory()->createHBox( vbox );
    hbox->setWeight( YD_VERT, 1 );

    YWidget * frame = YUI::widgetFactory()->createFrame( hbox, "System View" );
    {
        frame->setWeight( YD_HORIZ, 1 );
        frame		= YUI::widgetFactory()->createHVCenter( frame );
        frame		= YUI::widgetFactory()->createVBox( frame );
        auto tree		= YUI::widgetFactory()->createTree( atLeft(frame), "");
        tree->setNotify( true );
        {
            YItemCollection items;
            auto t = new YTreeItem( "Hard Disks" );
            for(auto& it : ansible_devices.items())
            {
                items.push_back( (t = new YTreeItem(it.key() )) );
                json partitions = it.value()["partitions"];
                for(auto& at : partitions.items())
                {
                    json disk_label = at.value()["links"]["labels"];
                    if (disk_label.empty())
                    {
                        disk_label[0] = "Undefined";
                    }
                    new YTreeItem( t, at.key() + " with size " + string(at.value()["size"]) + " and label " + string(disk_label[0]));
                }

            }
            tree->addItems( items ); // This is more efficient than repeatedly calling cbox->addItem
        }
    }
    frame			= YUI::widgetFactory()->createFrame( hbox, "Table" );
    {
        frame->setWeight( YD_HORIZ, 1 );
        frame		= YUI::widgetFactory()->createHVCenter( frame );
        frame		= YUI::widgetFactory()->createVBox( frame );

        auto head		= new YTableHeader;
        head->addColumn( "Right", YAlignEnd );
        head->addColumn( "Center", YAlignCenter );
        head->addColumn( "Left", YAlignBegin );
        auto table		= YUI::widgetFactory()->createTable( atLeft(frame), head );
        table->setNotify( true );

        //NEXT: when selecting disk or partition in left pane this area should show information.
        YItemCollection items;
        items.push_back( new YTableItem( "a", "b", "c", "extra" ) );
        items.push_back( new YTableItem( "aa", "bb", "cc" ) );
        items.push_back( new YTableItem( "aaa", "bbb", "ccc" ) );
        items.push_back( new YTableItem( "aaaa", "bbbb", "cccc" ) );
        items.push_back( new YTableItem( "aaaaa", "bbbbb", "ccccc" ) );
        items.push_back( new YTableItem( "aaaaaa", "bbbbbb", "cccccc" ) );
        table->addItems( items ); // This is more efficient than repeatedly calling cbox->addItem
    }

    /*YDialog    * dialog = YUI::widgetFactory()->createPopupDialog();
    YLayoutBox * vbox   = YUI::widgetFactory()->createVBox( dialog );
    YUI::widgetFactory()->createLabel     ( vbox, "Hello, World!" );
    YUI::widgetFactory()->createPushButton( vbox, "&OK" );
    */
    dialog->waitForEvent();
    dialog->destroy();
}


