package brm;

import javax.microedition.midlet.*;
import javax.microedition.lcdui.*;

public class BluetoothMIDlet extends MIDlet implements CommandListener
{
    private Command cmdExit, cmdSelect;
    private Display display;

    private List mainMenu;

    private static BluetoothMIDlet instance = null;

    public BluetoothMIDlet()
    {
        if (instance == null) instance = this;

        display = Display.getDisplay(this);
        cmdExit = new Command("Exit", Command.EXIT, 0);
        cmdSelect = new Command("Select", Command.SCREEN, 0);
    }

    public static BluetoothMIDlet instance()
    {
        return BluetoothMIDlet.instance;
    }

    protected void populateMenu()
    {
        mainMenu.deleteAll();

        if (Settings.instance().lastConnection.length() > 0) mainMenu.append("Use last connection", null);
        mainMenu.append("Connect to a device", null);
        mainMenu.append("Options", null);
        if (Console.DEBUG_ON) mainMenu.append("Debug console", null);
        mainMenu.append("Exit", null);
    }

    public void startApp()
    {
        mainMenu = new List("Brm BT Remote", List.IMPLICIT);
        populateMenu();

        mainMenu.addCommand(cmdExit);
        mainMenu.addCommand(cmdSelect);
        mainMenu.setCommandListener(this);

        display.setCurrent(mainMenu);
    }

    public void pauseApp()
    {
    }

    public void destroyApp(boolean unconditional)
    {
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdSelect || c == List.SELECT_COMMAND)
        {
            boolean hasLastConnection = mainMenu.getString(0).compareTo("Use last connection") == 0;
            int index = mainMenu.getSelectedIndex();

            if (index == 0 && hasLastConnection) // use last connection
            {
                new Communicator().launch(Settings.instance().lastConnection, mainMenu);
            }
            else if ( index == 0 && !hasLastConnection
                      || index == 1 && hasLastConnection) // find a device
            {
                DeviceList devList = new DeviceList();
                devList.findDevices();
            }
            else if ( index == 1 && !hasLastConnection
                      || index == 2 && hasLastConnection) // options
            {
                new Options().show();
            }
            else if ( index == 2 && !hasLastConnection && Console.DEBUG_ON
                      || index == 3 && hasLastConnection && Console.DEBUG_ON) // console
            {
                Console.instance().show();
            }
            else /*if (index - offset == 3)*/ // exit
            {
                destroyApp(false);
                notifyDestroyed();
            }
        }
        else if (c == cmdExit)
        {
            destroyApp(false);
            notifyDestroyed();
        }
    }

    public void show()
    {
        if ( mainMenu.getString(0).compareTo("Use last connection") != 0
             && Settings.instance().lastConnection.length() > 0) populateMenu(); // adds "Use Last Connection" to the menu (if not present)
        display.setCurrent(mainMenu);
    }

}
