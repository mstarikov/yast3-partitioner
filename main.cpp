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


string runAnsible(string arguments)
{

    string program = "/usr/bin/ansible ";
    string result = "";
    program = program.append(arguments);
    //convert program path with arguments into char array for popen
    const char *command = program.c_str();
    //FILE *program_output =
    popen(command, "r");
    /*char buffer[256];
    while(fgets(buffer, sizeof(buffer), program_output) != NULL)
    {
        if(string(buffer).find("localhost | SUCCESS") == 0)
        {
             // we have successful ansible command, skipping header
        }
        else
        {
           result += buffer;
        }

    }
    //TODO check if command is successful.
    return result;*/
}

void parseAnsible(string json_text)
{
    ifstream in("./ansible_facts.json");
    json fact_object = json::parse(in);
    cout << ("This is facts: %s \n",  fact_object["ansible_facts"]) << endl;
    //Convert stdout to JSON
    /*QJsonParseError jerror;
    QJsonDocument facts = QJsonDocument::fromJson(json_text.toLatin1(), &jerror);
    if (jerror.error != QJsonParseError::NoError)
    {
        qDebug() << "Error happened:" << jerror.errorString();
    }
    //Get object from Document & Unpack main ansible_facts section
    QJsonObject facts_object = facts.object()["ansible_facts"].toObject();
    return facts_object;*/
}

int main( int argc, char **argv )
{

    string json_string = runAnsible("localhost -m setup | sed -e 's/localhost | SUCCESS =>/''/g' > ./ansible_facts.json");
    parseAnsible(json_string);
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
            auto t = new YTreeItem( "Item 1" );
            items.push_back( t );

            items.push_back( (t = new YTreeItem( "Item 12" )) );
            new YTreeItem( t, "Item 1" );
            items.push_back( (t = new YTreeItem( "Item 123" )) );
            new YTreeItem( t, "Item 1" );
            t = new YTreeItem( t, "Item 2" );
            new YTreeItem( t, "Item 1" );
            items.push_back( (t = new YTreeItem( "Item 1234" )) );
            new YTreeItem( t, "Item 1" );
            new YTreeItem( t, "Item 2" );
            t = new YTreeItem( t, "Item 3" );
            new YTreeItem( t, "Item 1" );
            new YTreeItem( t, "Item 2" );
            items.push_back( (t = new YTreeItem( "Item 12345" )) );
            new YTreeItem( t, "Item 1" );
            new YTreeItem( t, "Item 2" );
            new YTreeItem( t, "Item 3" );
            t = new YTreeItem( t, "Item 4" );
            new YTreeItem( t, "Item 1" );
            new YTreeItem( t, "Item 2" );
            new YTreeItem( t, "Item 3" );
            items.push_back( (t = new YTreeItem( "Item 123456" )) );
            new YTreeItem( t, "Item 1" );
            new YTreeItem( t, "Item 2" );
            new YTreeItem( t, "Item 3" );
            new YTreeItem( t, "Item 4" );
            t = new YTreeItem( t, "Item 5" );
            new YTreeItem( t, "Item 1" );
            new YTreeItem( t, "Item 2" );
            new YTreeItem( t, "Item 3" );
            new YTreeItem( t, "Item 4" );
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


