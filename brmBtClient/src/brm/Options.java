/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package brm;

import javax.microedition.lcdui.*;

/**
 * Form for changing the settings
 * @author 81403
 */
public class Options implements CommandListener
{
    protected Form form;
    protected ChoiceGroup backlight;
    protected TextField autoDisconnect;
    protected Command cmdBack;
    
    public Options()
    {
        form = new Form("Options");

        backlight = new ChoiceGroup("Force backlight on", ChoiceGroup.EXCLUSIVE);
        backlight.append("Yes", null);
        backlight.append("No", null);
        backlight.setSelectedIndex(Settings.instance().forceBacklight ? 0 : 1, true);
        form.append(backlight);

        StringItem help1 = new StringItem("*may not work on some devices\n", "");
        help1.setFont(Font.getFont(Font.FACE_SYSTEM, Font.STYLE_ITALIC, Font.SIZE_SMALL));
        form.append(help1);

        autoDisconnect = new TextField("Auto disconnect after", new Integer(Settings.instance().autoDisconnect).toString(), 3, TextField.DECIMAL);
        form.append(autoDisconnect);

        StringItem help2 = new StringItem("0 = do not disconnect automatically", "");
        help2.setFont(Font.getFont(Font.FACE_SYSTEM, Font.STYLE_ITALIC, Font.SIZE_SMALL));
        form.append(help2);

        cmdBack = new Command("Back", Command.BACK, 0);
        form.addCommand(cmdBack);
        form.setCommandListener(this);
    }

    public void commandAction(Command c, Displayable d)
    {
        if (c == cmdBack)
        {
            Settings.instance().forceBacklight = backlight.isSelected(0);
            Settings.instance().autoDisconnect = Integer.parseInt(autoDisconnect.getString());
            Settings.instance().save();
            BluetoothMIDlet.instance().show();
        }
    }

    public void show()
    {
        Display.getDisplay(BluetoothMIDlet.instance()).setCurrent(form);
    }

}
