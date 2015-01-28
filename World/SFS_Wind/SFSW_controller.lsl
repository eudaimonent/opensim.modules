//
// A Simple Fluid Solver Wind Controller
//
//


integer DEFAULT_CHANNEL = 6000;

integer cmd_channel   = 0;
integer listen_hdl    = 0;



// 初期化
init()
{
    cmd_channel  = DEFAULT_CHANNEL;
 
    if (listen_hdl!=0) llListenRemove(listen_hdl);
    listen_hdl = llListen(cmd_channel, "", NULL_KEY, "");
}



//
// Default State
// 

default
{
    listen(integer ch, string name, key id, string msg) 
    {
        if (listen_hdl!=0) llListenRemove(listen_hdl);
        llSay(0, "Recived: " + msg);

        list items = llParseString2List(msg, ["=", ",", " ","\n"], []);
        string cmd = llList2String(items, 0);
        
        if (cmd=="strength") {
            string param = llList2String(items, 1);
            osSetWindParam("SimpleFluidSolverWind", "strength", (float)param);
            llSay(0, "set paramader : "+cmd+" = "+param);
        }

        else if (cmd=="damping") {
            string param = llList2String(items, 1);
            osSetWindParam("SimpleFluidSolverWind", "damping", (float)param);
            llSay(0, "set paramader : "+cmd+" = "+param);
        }

        else if (cmd=="force") {
            string param = llList2String(items, 1);
            osSetWindParam("SimpleFluidSolverWind", "force", (float)param);
            llSay(0, "set paramader : "+cmd+" = "+param);
        }
        
        else if (cmd=="region") {
            string param = llList2String(items, 1);
            osSetWindParam("SimpleFluidSolverWind", "region", (float)param);
            llSay(0, "set paramader : "+cmd+" = "+param);
        }

        else if (cmd=="reset") {
            init();
        }

        else if (cmd=="help") {
            llSay(0, "/"+(string)cmd_channel+" force 数値");
            llSay(0, "/"+(string)cmd_channel+" strength 数値");
            llSay(0, "/"+(string)cmd_channel+" damping 数値");
            llSay(0, "/"+(string)cmd_channel+" region 数値");
            llSay(0, "/"+(string)cmd_channel+" help");
            llSay(0, "/"+(string)cmd_channel+" reset");
        }

        else {
            llSay(0, "Unknown Command: " + cmd);
        }
        
        if (listen_hdl!=0) llListenRemove(listen_hdl);
        listen_hdl = llListen(cmd_channel, "", NULL_KEY, "");
    }


    changed(integer change)
    {
        if (change & CHANGED_REGION_START) {
            llResetScript();
        }       
    }


    state_entry()
    {
        init();
    }


    on_rez(integer start_param) 
    {
        llResetScript();
    }


    touch_start(integer total_number)
    {
        llSay(0, "Force Kind = "+(string)((integer)osGetWindParam("SimpleFluidSolverWind", "force")));
        llSay(0, "Region Size = "+(string)((integer)osGetWindParam("SimpleFluidSolverWind", "region")));
        llSay(0, "Strength = "+(string)osGetWindParam("SimpleFluidSolverWind", "strength"));
        llSay(0, "Damping Rate = "+(string)osGetWindParam("SimpleFluidSolverWind", "damping"));
        llSay(0, "Command Channel = "+(string)cmd_channel);
    }
}

