package brm;

import javax.microedition.lcdui.*;

/**
 * Debugging console
 * To enable or disable it, change DEBUG_ON to true or false
 * Watch out: the console considerably slows down the app, use it only for debugging!
 */
public class Console implements CommandListener
{
    private static Console instance = null;
    private Form console;
    private Command cmdBack, cmdClear;

    public static final boolean DEBUG_ON = false; // set this to true to enable the debugging console

    private Console()
    {
        if (Console.instance == null) Console.instance = this;

        cmdBack = new Command("Back", Command.BACK, 0);
        cmdClear = new Command("Clear log", Command.SCREEN, 0);

        console = new Form("Debug console");
        console.addCommand(cmdBack);
        console.addCommand(cmdClear);
        console.setCommandListener(this);
    }

    public static Console instance()
    {
        if (instance == null) return new Console();
        else return instance;
    }

    public void show()
    {
        Display.getDisplay(BluetoothMIDlet.instance()).setCurrent(console);
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdBack)
        {
           BluetoothMIDlet.instance().show();
        }
        else if (c == cmdClear)
        {
            console.deleteAll();
        }
    }

    public void log (String s)
    {
        if (DEBUG_ON) console.append(s);
    }

    public void log (int i)
    {
        if (DEBUG_ON) console.append(new Integer(i).toString());
    }
}
