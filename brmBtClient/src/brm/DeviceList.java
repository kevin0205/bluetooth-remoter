package brm;

import javax.microedition.lcdui.*;
import java.io.*;
import java.util.Enumeration;
import java.util.Vector;
import javax.bluetooth.*;

/**
 * Detects nearby devices (gets names, detects if the brm bt server is running and gets the connection string) and connects to them
 */
public class DeviceList implements CommandListener, DiscoveryListener
{
    protected Console console;
    protected String connectionURL="";

    protected List deviceList;

    protected Command cmdBack, cmdAbort, cmdRescan;

    protected java.util.Vector devices, services;
    protected LocalDevice local;
    protected DiscoveryAgent agent;

    public DeviceList()
    {
        console = Console.instance();

        cmdBack = new Command("Back", Command.BACK, 0);
        cmdAbort = new Command("Abort scan", Command.CANCEL, 0);
        cmdRescan = new Command("Rescan devices", Command.SCREEN, 0);

        deviceList = new List("Available bluetooth devices", List.IMPLICIT);
        deviceList.addCommand(cmdBack);
        deviceList.addCommand(cmdRescan);
        deviceList.setCommandListener(this);

        devices = new Vector();
        services = new Vector();
    }

    public void commandAction(Command c, Displayable s)
    {
        if (c == cmdBack)
        {
            BluetoothMIDlet.instance().show();
        }
        else if (c == cmdRescan)
        {
            findDevices();
        }
        else if (c == cmdAbort)
        {
            console.log("Device scan aborted\n");
            agent.cancelInquiry(this);
        }
        else if (c == List.SELECT_COMMAND)
        {
            findServicesAndConnect( (RemoteDevice) devices.elementAt(deviceList.getSelectedIndex()) );
        }
    }

    public void findDevices()
    {
        deviceList.deleteAll();
        devices.removeAllElements();

        Display display = Display.getDisplay(BluetoothMIDlet.instance());

        display.setCurrent(deviceList);

        Gauge inqGauge = new Gauge(null, false, Gauge.INDEFINITE, Gauge.CONTINUOUS_RUNNING);
        Alert scanAlert = new Alert(null, "Searching for Devices...", null,  null);
        scanAlert.setTimeout(Alert.FOREVER); // until inquiryCompleted is called

        scanAlert.setIndicator(inqGauge);
        scanAlert.addCommand(cmdAbort);
        scanAlert.setCommandListener(this);

        display.setCurrent(scanAlert, deviceList);

        console.log("Searching for devices...\n");
        try
        {
            devices = new java.util.Vector();
            local = LocalDevice.getLocalDevice();
            agent = local.getDiscoveryAgent();
            agent.startInquiry(DiscoveryAgent.GIAC,this);
        }
        catch(Exception e)
        {
            console.log("Error in initiating device scan - "+e.getMessage());
            this.do_alert("Error in initiating device scan.\nCheck if Bluetooth is enabled on your phone.");
        }
 }

    public void findServicesAndConnect(RemoteDevice device)
    {
        console.log("Inquirying services for device "+deviceList.getString(deviceList.getSelectedIndex())+"\n");

        Gauge inqGauge = new Gauge(null, false, Gauge.INDEFINITE, Gauge.CONTINUOUS_RUNNING);
        Alert scanAlert = new Alert(null, "Connecting...", null,  null);
        scanAlert.setTimeout(Alert.FOREVER);

        scanAlert.setIndicator(inqGauge);
        scanAlert.setCommandListener(this);

        Display display = Display.getDisplay(BluetoothMIDlet.instance());
        display.setCurrent(scanAlert, deviceList);

        try
        {
            UUID[] uuids  = new UUID[1];
            uuids[0]      = new UUID("8140300013dfb0007cf4350b4a2110ee",false);
            agent.searchServices(null,uuids,device,this);
        }
        catch(Exception e)
        {
            this.do_alert("Error in initiating services search - "+e.getMessage());
        }
    }

    public void deviceDiscovered(RemoteDevice remoteDevice,DeviceClass deviceClass)
    {
        devices.addElement(remoteDevice);
        deviceList.append("Retrieving name... ("+remoteDevice.getBluetoothAddress()+")", null);
    }

    public void servicesDiscovered(int transID,ServiceRecord[] serviceRecord)
    {
        for (int x = 0; x < serviceRecord.length; x++ )
        {
            services.addElement(serviceRecord[x]);
        }
    }

    public void inquiryCompleted(int param)
    {
        Display.getDisplay(BluetoothMIDlet.instance()).setCurrent(deviceList);
        
        switch (param)
        {
            case DiscoveryListener.INQUIRY_TERMINATED:  // Inquiry terminated by agent.cancelInquiry()
            case DiscoveryListener.INQUIRY_COMPLETED:
                if (devices.size() > 0)
                {
                    //services = new java.util.Vector();
                    //this.FindServices((RemoteDevice) devices.elementAt(0));
                    int i = 0;
                    for (Enumeration enm = devices.elements(); enm.hasMoreElements();)
                    {
                        RemoteDevice rd = (RemoteDevice) enm.nextElement();
			try
                        {
                            deviceList.set(i, rd.getFriendlyName(true), null);
			}
                        catch (IOException e)
                        {
                            deviceList.set(i, "Unknown device #"+rd.getBluetoothAddress(), null);
                            console.log("Could not retrieve name for: "+rd.getBluetoothAddress()+"\n");
			}
                        i++;
                    }
                }
                else do_alert("No device found in range");
            break;
            case DiscoveryListener.INQUIRY_ERROR:       // Error during inquiry
                this.do_alert("Error occured while searching for devices.");
            break;
        }
    }

    public void serviceSearchCompleted(int transID, int respCode)
    {
        Display.getDisplay(BluetoothMIDlet.instance()).setCurrent(deviceList);

        connectionURL = "";
        
        switch(respCode)
        {
            case DiscoveryListener.SERVICE_SEARCH_COMPLETED:
                if(services.size() > 0)
                {
                    connectionURL = ((ServiceRecord) services.firstElement()).getConnectionURL(0,false);
                    console.log("Got URL:"+connectionURL+"\n");
                    Settings.instance().lastConnection = connectionURL;
                    Settings.instance().save();
                    
                    new Communicator().launch(connectionURL, deviceList);
                }
                else
                    do_alert("Service finder internal error."); // this can probably never happen
            break;
            case DiscoveryListener.SERVICE_SEARCH_DEVICE_NOT_REACHABLE:
                 this.do_alert("Device not reachable.");
            break;
            case DiscoveryListener.SERVICE_SEARCH_ERROR:
                 this.do_alert("Service search error.");
            break;
            case DiscoveryListener.SERVICE_SEARCH_NO_RECORDS:
                do_alert("Brm BT remote server is not running on the chosen device.");
            break;
            case DiscoveryListener.SERVICE_SEARCH_TERMINATED:
                //this.do_alert("SRV - Inquiry Cancelled");
            break;
        }
    }

    public void do_alert(String str)
    {
        Display display = Display.getDisplay(BluetoothMIDlet.instance());
        
        Alert a = new Alert("alert", str, null, AlertType.WARNING);
        a.setTimeout(Alert.FOREVER);
        display.setCurrent(a, deviceList);
    }
}
