package brm;

import javax.microedition.lcdui.*;
import javax.microedition.io.*;
import java.io.*;
import java.util.Date;
import java.util.Hashtable;
import javax.microedition.lcdui.game.Sprite;

/**
 * The core of Brm BT Remote - manages connection with the server,
 * sends & receives commands, displays available commands, loads profiles, ...
 * Needs to be launched with launch()
 * @author 81403
 */
public class Communicator extends Canvas implements CommandListener, Runnable
{
    protected String connectionURL; // connection URL is stored here and used when reconnecting
    protected Displayable prevDisp; // previous displayable to return to when exiting
    protected Command cmdBack, cmdChangeProfile, cmdCloseProfileList;
    protected boolean profileLoaded, disconnected, isConnecting, exitTimerThread;
    protected List profileList;

    protected InputStream input;
    protected OutputStream output;
    protected StreamConnection conn;

    protected int threadType; // run() will start a specific thread according to this; this sneaky variable is required because run() does not have any parameters
    protected Thread readerThread = null, timerThread = null;

    protected long lastActivity; // last user interaction with the app; used by timerThread for disconnecting after a period of inactivity in order to save power

    //
    // stuff for drawing buttons and whatnot:
    protected int layout = 1;
    protected Image gradient;
    protected Hashtable pressedKeys;
    protected int touchedKey;
    protected int keys[] = {-5, -1, -8, -3, -2, -4, 49, 50, 51, 52, 53, 54, 55, 56, 57, 42, 48, 35};
    protected String keyNames[] = { "OK", "▲", "C", "◄", "▼", "►", "1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#" };
    protected String keyLabels[] = { "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" ,"" };
    protected char keyColors[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    protected char colors[][] = { {0,0,0}, {0,30,10}, {0,30,40}, {40,30,0}, {40,40,0} };
    protected String field1 = "", field2 = "";

    protected int yOffset, width, height, cellSizeX, cellSizeY, firstKey, lastKey;
    //

    public Communicator()
    {
        disconnected = true;
        isConnecting = false;
        profileLoaded = false;
        cmdBack = new Command("Back", Command.BACK, 0);
        cmdChangeProfile = new Command("Change Profile", Command.SCREEN, 0);
        cmdCloseProfileList = new Command("Cancel", Command.BACK, 0);

        pressedKeys = new Hashtable();

        try {
            gradient = Image.createImage("/button_gradient.png");
        } catch (IOException e) {
            gradient = null;
        }

        this.addCommand(cmdBack);
        this.addCommand(cmdChangeProfile);
        this.setCommandListener(this);

        profileList = new List("Choose a profile", List.IMPLICIT);
        profileList.addCommand(cmdCloseProfileList);
        profileList.setCommandListener(this);
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdBack)
        {
            disconnect();
            exitTimerThread = true;
            Display.getDisplay(BluetoothMIDlet.instance()).setCurrent(prevDisp);
        }
        else if (c == cmdChangeProfile)
        {
            lastActivity = new Date().getTime();

            profileList.deleteAll();
            if (sendString("LIST PROFILES"))
            {
                Display.getDisplay(BluetoothMIDlet.instance()).setCurrent(profileList);
            }
        }
        else if (c == cmdCloseProfileList)
        {
            if (!profileLoaded)
            {
                exitTimerThread = true;
                disconnect();
                Display.getDisplay(BluetoothMIDlet.instance()).setCurrent(prevDisp);
            } // can't work without a profile
            else Display.getDisplay(BluetoothMIDlet.instance()).setCurrent(this);
        }
        else if (c == List.SELECT_COMMAND)
        {
            lastActivity = new Date().getTime();

            sendString("LOAD PROFILE "+profileList.getString(profileList.getSelectedIndex()));
            Settings.instance().lastProfile = profileList.getString(profileList.getSelectedIndex());
            Settings.instance().save();
        }
    }

    /**
     * Launches the communicator
     */
    public void launch(String URL, Displayable prevDisp)
    {
        this.prevDisp = prevDisp;
        this.connectionURL = URL;
        exitTimerThread = false;
        
        if (!connect(true)) return; // this will also return to the previous displayable

        if (Settings.instance().lastProfile.length() != 0)
        {
            sendString("LOAD PROFILE "+Settings.instance().lastProfile);
        }
        else
        {
            commandAction(cmdChangeProfile, this); // show the profile list
        }

        this.threadType = 1;
        timerThread = new Thread(this);
        timerThread.start();
    }

    /**
     * Establishes a connection to a device + creates a thread for receiving incoming messages
     * This method is synchronized because it may be called by timer thread when the device returns from power-saving mode and the user
     * could press a key while connecting, which would cause another simultaneous connect
     */
    protected synchronized boolean connect(boolean exitOnFailure)
    {
        if (!disconnected) return false; // already connected

        // notify the user that the connection is being established
        isConnecting = true;
        if (isShown()) repaint();

        Console.instance().log("Opening data streams...");

        try
        {
            conn = (StreamConnection) Connector.open(connectionURL);
            input = conn.openInputStream();
            output = conn.openOutputStream();
        }
        catch (IOException e)
        {
            Console.instance().log("Connection failed - "+e.getMessage()+"\n");
            disconnect();
            if (exitOnFailure)
            {
                exitTimerThread = true;
                do_alert("Could not open connection with the target device.", prevDisp);
            }
            else do_alert("Could not open connection with the target device.");

            isConnecting = false;
            return false;
        }

        disconnected = false;
        isConnecting = false;
        if (isShown()) repaint();

        this.threadType = 0;
        readerThread = new Thread(this);
        readerThread.start();

        return true;
    }

    /**
     * Closes current connection
     * (receiverThread exits automatically)
     */
    protected void disconnect()
    {
        if (disconnected) return;

        disconnected = true;
        if (isShown()) repaint();

        try
        {
            input.close();
            output.close();
            conn.close();
        }
        catch (IOException e)
        {
            Console.instance().log("Could not close connection - " + e.getMessage() + "\n");
            return;
        }

        Console.instance().log("Disconnected.\n");
    }

    /**
     * Sends a message via Bluetooth
     */
    protected boolean sendString(String s)
    {
        if (disconnected)
        {
            do_alert("Not connected!");
            return false;
        }

        try
        {
            output.write(s.getBytes());
            output.flush();
        }
        catch (IOException e)
        {
            do_alert("Failed to send the message!");
            Console.instance().log("Could not send the message - " + e.getMessage() + "\n");
            return false;
        }
        return true;
    }

    /**
     * Creates a thread according to threadType
     */
    public void run()
    {
        if (threadType == 0)
        {
            Console.instance().log("Created receiver thread\n");
            receiverThread();
            Console.instance().log("Exiting receiver thread\n");
        }
        else if (threadType == 1)
        {
            Console.instance().log("Created timer thread\n");
            timerThread();
            Console.instance().log("Exiting timer thread\n");
        }
        threadType = -1;
    }

    /**
     * Parses received data and loads list of profiles into profileList
     */
    protected void loadProfiles(String txt)
    {
        // this horrible mess is just for reading the string by lines since Java ME CAN'T DO IT BY ITSELF!
        // we'll skip the first line, which bears the header
        String line = txt.substring(0, txt.indexOf('\n') == -1 ? txt.length() : txt.indexOf('\n'));
        txt = txt.substring(txt.indexOf('\n') == -1 ? txt.length() : txt.indexOf('\n')+1);

        while (txt.length() != 0)
        {
            line = txt.substring(0, txt.indexOf('\n') == -1 ? txt.length() : txt.indexOf('\n'));
            txt = txt.substring(txt.indexOf('\n') == -1 ? txt.length() : txt.indexOf('\n')+1);

            if (line.length() != 0) profileList.append(line, null);
        }
    }

    /**
     * Parses received data and loads a profile (keys, colors and layout)
     */
    protected void loadProfile(String profile)
    {
        for (int i = 0; i < 18; i++) { keyLabels[i] = null; keyColors[i] = 0; }
        layout = 1;
        
        // this mess does the same thing as in the function above.. except it loads a profile instead of a list
        String line = profile.substring(0, profile.indexOf('\n') == -1 ? profile.length() : profile.indexOf('\n'));
        profile = profile.substring(profile.indexOf('\n') == -1 ? profile.length() : profile.indexOf('\n')+1);

        while (profile.length() != 0)
        {
            line = profile.substring(0, profile.indexOf('\n') == -1 ? profile.length() : profile.indexOf('\n'));
            profile = profile.substring(profile.indexOf('\n') == -1 ? profile.length() : profile.indexOf('\n')+1);

           String segments[] = Settings.explodeString(line, "\t");
           if (segments[0].compareTo("Layout:") == 0)
           {
               layout = Integer.parseInt(segments[1]);
               if (layout < 1 || layout > 3) layout = 1;
           }
           else if(segments[0].compareTo("Key:") == 0)
           {
               int i, key = Integer.parseInt(segments[1]);
               for (i = 0; ; i++) if (i == keys.length || keys[i] == key) break;
               if (i != keys.length)
               {
                   keyLabels[i] = segments[2];
                   if (segments[3].compareTo("Green") == 0) keyColors[i] = 1;
                   else if (segments[3].compareTo("Teal") == 0) keyColors[i] = 2;
                   else if (segments[3].compareTo("Orange") == 0) keyColors[i] = 3;
                   else if (segments[3].compareTo("Yellow") == 0) keyColors[i] = 4;
                   else keyColors[i] = 0;
               }
           }

        }
    }

    /**
     * Receives messages via Bluetooth and acts accordingly
     * Also notifies the user if the connection was unexpectedly interrupted (by server, getting out of range, etc)
     * This thread runs only if a connection is active
     */
    protected void receiverThread()
    {
        byte b[] = new byte[4096];
        int bytesRead = 0;
        int dataSize = 0;

        try
        {

            bytesRead = input.read(b, 0, 4);
            Console.instance().log("Bytes read:" + bytesRead + "\n");

            while (bytesRead > 0) // while the stream is open...
            {
                dataSize = 0;    dataSize |= b[3] & 0xFF;
                dataSize <<= 8;  dataSize |= b[2] & 0xFF;
                dataSize <<= 8;  dataSize |= b[1] & 0xFF;
                dataSize <<= 8;  dataSize |= b[0] & 0xFF;

                Console.instance().log("Data size:" + dataSize + "\n");

                int totalRead = 0;
                while (totalRead < dataSize)
                {
                    bytesRead = input.read(b, totalRead, dataSize - totalRead > 256 ? 256 : dataSize - totalRead);
                    totalRead += bytesRead;
                    // Console.instance().log("totalRead:" + totalRead + "\n");
                }

                String txt = new String(b, 0, totalRead, "UTF-8");
                Console.instance().log("\n---\n");
                Console.instance().log(txt);

                if (txt.length() >= 8 && txt.substring(0,8).compareTo("PROFILE:") == 0)
                {
                    loadProfile(txt);
                    Display.getDisplay(BluetoothMIDlet.instance()).setCurrent(this);
                    this.setTitle(txt.substring(9, txt.indexOf("\n")).trim());
                    profileLoaded = true;
                }
                else if (txt.length() >= 13 && txt.substring(0,13).compareTo("PROFILE ERROR") == 0)
                {
                    // refresh the list.. it might have been updated by user on the server
                    // or it's not yet populated (in case the last used profile failed to load)
                    profileList.deleteAll();
                    sendString("LIST PROFILES");

                    do_alert("Profile "+Settings.instance().lastProfile+" could not be loaded!", profileList);
                }
                else if (txt.length() >= 13 && txt.substring(0,13).compareTo("PROFILE LIST:") == 0)
                {
                    loadProfiles(txt);
                }
                else if (txt.length() >= 7 && txt.substring(0,7).compareTo("FIELD1:") == 0)
                {
                    field1 = txt.substring(8);
                    if (isShown()) repaint();
                }
                else if (txt.length() >= 7 && txt.substring(0,7).compareTo("FIELD2:") == 0)
                {
                    field2 = txt.substring(8);
                    if (isShown()) repaint();
                }

                bytesRead = input.read(b, 0, 4);
            }
        }
        catch (IOException e)
        {
            Console.instance().log("Error reading input stream - " + e.getMessage() + "\n");
        }

        if (!disconnected) // the connection was closed by the server or interrupted
        {
            do_alert("Connection interrupted!");
            disconnect();
        }
    }

    /**
     * Disconnects automatically in order to save power and keeps the backlight on (both according to user's preferences)
     * Also, automatically reconnects after returning from an inactive state (app was switched to background, backlight went off, etc.)
     * This thread runs for the whole lifespan of the Communicator object
     */
    protected void timerThread()
    {
        lastActivity = new Date().getTime();
        boolean wasShown = true;
        
        while (!exitTimerThread)
        {
            if (wasShown == false && isShown() == true && disconnected
                 && (new Date().getTime() - lastActivity)/1000 > Settings.instance().autoDisconnect) // the application regained focus after losing it (backlight went off, etc.)
            {
                lastActivity = new Date().getTime();

                if (connect(false))
                {
                    if (sendString("LOAD PROFILE "+Settings.instance().lastProfile))
                        try { Thread.sleep(500); } catch (Exception e) {} // wait for a while, so we can be more or less sure, that the server has loaded the profile
                }
            }

            wasShown = isShown();

            if (Settings.instance().autoDisconnect > 0) // disconnect after a period of inactivity to save power
            {
                if ((new Date().getTime() - lastActivity)/1000 > Settings.instance().autoDisconnect) disconnect();
            }

            if (Settings.instance().forceBacklight) // (try to) force the backlight on
            {
                Display.getDisplay(BluetoothMIDlet.instance()).flashBacklight(1);
                Display.getDisplay(BluetoothMIDlet.instance()).flashBacklight(0);
            }

            try { Thread.sleep(1000); } catch (Exception e) { return; }
        }
    }

    /**
     * Sets metrics for a layout (count, width, height of a cell, ...)
     * Called when painting the screen and when converting cursor position of a touch device into key stroke
     */
    protected void setLayoutMetrics()
    {
        if (layout == 1)
        {
            Font small = Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_PLAIN, Font.SIZE_SMALL);
            yOffset = small.getHeight()*2+6;
            width = getWidth();  height = getHeight() - yOffset;
            cellSizeX = width / 3;  cellSizeY = height / 4;
            firstKey = 6;  lastKey = 18; // 1 through #
        }
        else if (layout == 2)
        {
            yOffset = 0;
            width = getWidth();  height = getHeight() - yOffset;
            cellSizeX = width / 3;  cellSizeY = height / 6;
            firstKey = 0;  lastKey = 18; // all the keys
        }
        else
        {
            yOffset = (getHeight() / 5)*3;
            width = getWidth();  height = getHeight() - yOffset;
            cellSizeX = width / 3;  cellSizeY = getHeight() / 5;
            firstKey = 0;  lastKey = 6; // arrows only
        }
    }

    /**
     * All the drawing happens here
     */
    public void paint(Graphics g)
    {
        Font small = Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_PLAIN, Font.SIZE_SMALL);
        Font large = Font.getFont(Font.FACE_PROPORTIONAL, Font.STYLE_BOLD, Font.SIZE_LARGE);

        setLayoutMetrics();

       if (layout == 1 || layout == 3)
        {
            // background
            g.setColor(168, 204, 141);
            g.fillRect(3, 3, getWidth()-6, yOffset-3);
            // gloss
            // g.setColor(179, 211, 148);
            // int third = getWidth() / 3;
            // g.fillTriangle(third, 0, third+third, 0, 0, third);
            // g.fillTriangle(third+third, 0, 0, third, 0, third+third);
            // inner part of the shade
            g.setColor(155, 189, 129);
            g.drawRect(2, 2, getWidth()-5, yOffset-2);
            // central part of the shade
            g.setColor(133, 166, 107);
            g.drawRect(1, 1, getWidth()-3, yOffset);
            // outer part of the shade
            g.setColor(110, 142, 85);
            g.drawRect(0, 0, getWidth()-1, yOffset);
        }

        g.setColor(30,30,30);
        g.fillRect(0, yOffset, width, height);

        for (int i = firstKey; i < lastKey; i++)
        {
            int row = (i-firstKey) / 3, col = (i-firstKey) % 3;
            int pressedOffset = pressedKeys.get(new Integer(keys[i])) != null ? 1 : 0;

            /*if (keyLabels[i] == null)
            {
                g.setColor(0, 0, 0);
                g.fillRect(col*cellSizeX+1, row*cellSizeY+1+yOffset, cellSizeX-1, cellSizeY-1);
                continue;
            }*/

            if (pressedOffset != 0) g.setColor(colors[keyColors[i]][0]+25,colors[keyColors[i]][1]+25,colors[keyColors[i]][2]+25);
            else g.setColor(colors[keyColors[i]][0],colors[keyColors[i]][1],colors[keyColors[i]][2]);

            g.fillRect(col*cellSizeX+1, row*cellSizeY+1+yOffset, cellSizeX-1, cellSizeY-1);

            g.setFont(large);
            g.setColor(60,60,60);
            g.drawString(keyNames[i], (col+1)*cellSizeX-5, (row+1)*cellSizeY-3+yOffset+pressedOffset, Graphics.BOTTOM | Graphics.RIGHT);
            g.setFont(small);
            g.setColor(140,140,140);
            // g.drawString(keyLabels[i], col*cellSizeX+(cellSizeX/2), row*cellSizeY+(cellSizeY/2)+yOffset-3, Graphics.BASELINE | Graphics.HCENTER);
            if (keyLabels[i] != null) WrappedTextWriter.write(g, keyLabels[i],
                    col*cellSizeX+(cellSizeX/2), row*cellSizeY+(cellSizeY/2)+yOffset-small.getHeight()+pressedOffset,
                    cellSizeX, small, Graphics.HCENTER);

           g.drawRegion(gradient, 0, (gradient.getHeight()-cellSizeY)/2-pressedOffset, cellSizeX, cellSizeY, Sprite.TRANS_NONE,
               col*cellSizeX, row*cellSizeY+yOffset, Graphics.TOP | Graphics.LEFT);
        }

        if (layout == 1)
        {
            g.setFont(small);
            g.setColor(51, 71, 31); // g.setColor(76, 103, 49);
            g.drawString(field1, 2, 2, Graphics.TOP | Graphics.LEFT);
            g.drawString(field2, 2, 4+small.getHeight(), Graphics.TOP | Graphics.LEFT);
        }
        else if (layout == 3)
        {
            g.setFont(small);
            g.setColor(51, 71, 31); // g.setColor(76, 103, 49);
            WrappedTextWriter.write(g, field1+"\n\n"+field2,
                    2, 2,
                    getWidth()-4, small, Graphics.LEFT);
        }

        if (disconnected)
        {
            g.setColor(0,0,0);
            g.fillRect(0, getHeight()-small.getHeight()-4, getWidth(), getHeight());
            g.setFont(small);
            g.setColor(255,128,128);
            g.drawString(isConnecting ? "connecting..."  : "disconnected (press a key)", getWidth()/2, getHeight()-2, Graphics.HCENTER | Graphics.BOTTOM);
        }
    }

    protected void keyPressed(int keyCode)
    {
        lastActivity = new Date().getTime();

        if (disconnected)
        {
            // this will halt the whole main thread until the connection either succeeds or fails..
            // which means that if you press a bunch of keys, they get buffered e.g. keyPressed() and Released() never get called simultaneously
            if (connect(false))
            {
                if (sendString("LOAD PROFILE "+Settings.instance().lastProfile))
                    try { Thread.sleep(500); } catch (Exception e) {} // wait for a while, so we can be more or less sure, that the server has loaded the profile
            }
        }

        pressedKeys.put(new Integer(keyCode), new Integer(1));

        repaint();
        sendString("DN:"+keyCode);
    }

    protected void keyReleased(int keyCode)
    {
        lastActivity = new Date().getTime();

        pressedKeys.remove(new Integer(keyCode));
        repaint();
        sendString("UP:"+keyCode);
    }

    /**
     * Converts touch input into a key stroke
     */
    protected void pointerPressed(int x, int y)
    {
        setLayoutMetrics();

        if (y<=yOffset) return;

        touchedKey = keys[firstKey + (x / cellSizeX + (3*((y-yOffset)/cellSizeY)))];
        keyPressed(touchedKey);
    }

    /**
     * Converts touch input into a key stroke (release)
     */
    protected void pointerReleased(int x, int y)
    {
        keyReleased(touchedKey);
    }

    /**
     * Generates an alert
     */
    protected void do_alert(String str)
    {
        Display display = Display.getDisplay(BluetoothMIDlet.instance());

        Alert a = new Alert("alert", str, null, AlertType.WARNING);
        a.setTimeout(Alert.FOREVER);
        display.setCurrent(a, this);
    }

    /**
     * Generates an alert and changes the displayable to "d"
     */
    protected void do_alert(String str, Displayable d)
    {
        Display display = Display.getDisplay(BluetoothMIDlet.instance());

        Alert a = new Alert("alert", str, null, AlertType.WARNING);
        a.setTimeout(Alert.FOREVER);
        display.setCurrent(a, d);
    }
}
