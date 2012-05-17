package brm;

import javax.microedition.rms.RecordStore;

/**
 * Stores user's preferences in RecordStore (and provides the explodeString() method)
 * This class is a singleton
 * @author 81403
 */
public class Settings
{
    protected static final String storageName = "brmBtStore";
    protected static Settings instance = null;

    public String lastConnection = "";
    public int autoDisconnect = 30;
    public boolean forceBacklight = false;
    public String lastProfile = "";

    protected Settings()
    {
        load();
    }

    public static Settings instance()
    {
        if (instance == null) instance = new Settings();
        return instance;
    }

    /**
     * Explodes a string into an array of strings (max. 10)
     */
    public static String[] explodeString(String str, String delimiter)
    {
        String values[] = {"", "", "", "", "", "", "", "", "", ""};

        int index = str.indexOf(delimiter), count = 0;

        while(index >= 0 && count < values.length)
        {
            values[count] = str.substring(0, index);
            str = str.substring(index+delimiter.length());
            index = str.indexOf(delimiter);
            count++;
        }

        if (str.length() > 0 && count < values.length) values[count] = str;

        return values;
    }

    public boolean load()
    {
        String cfg;

        try
        {
            RecordStore rs = RecordStore.openRecordStore(storageName, true);

            if (rs.getNumRecords() == 0)
            {
                Console.instance().log("No settings loaded, using defaults");
                rs.closeRecordStore();
                return false;
            }

             byte b[] = rs.getRecord(1);
            cfg = new String(b, 0, b.length);

            rs.closeRecordStore();
        }
        catch (Exception e)
        {
            return false;
        }

        Console.instance().log("Loaded settings: "+cfg);

        String[] values = explodeString(cfg, "|||");
        this.lastConnection = values[0];
        this.autoDisconnect = Integer.parseInt(values[1]);
        this.forceBacklight = (Integer.parseInt(values[2]) > 0 ? true : false);
        this.lastProfile = values[3];

        return true;
    }

    public boolean save()
    {
        byte[] cfg = (lastConnection+"|||"+autoDisconnect+"|||"+(forceBacklight?1:0)+"|||"+this.lastProfile).getBytes();

        // deleting and recreating the record store ensures, that we'll always have only one record with id 1
        try
        {
            RecordStore.deleteRecordStore(storageName);
        }
        catch (Exception e) { }

        try
        {
            RecordStore rs = RecordStore.openRecordStore(storageName, true);
            rs.addRecord(cfg, 0, cfg.length);
            rs.closeRecordStore();
        }
        catch (Exception e)
        {
            return false;
        }

        Console.instance().log("Settings saved!");

        return true;
    }
}

