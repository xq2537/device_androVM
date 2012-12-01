package com.androVM.vmconfig;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.Writer;
import java.util.HashMap;
import java.util.Scanner;

public class androVMTextConfig {
	private String GetPropCmd = "/system/bin/androVM-prop get ";
	private String SetPropCmd = "/system/bin/androVM-prop set ";
	private String RmPropCmd = "/system/bin/androVM-prop rm ";
	private String GetIpInfoCmd = "ifconfig eth0";
	
	private String execNativeCommand(String command) {
		try {
			Process process = Runtime.getRuntime().exec(command);
			
			BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
			int read;
			char[] buffer = new char[4096];
			StringBuffer output = new StringBuffer();
			while ((read = reader.read(buffer)) >0) {
				output.append(buffer, 0, read);
			}
			reader.close();
			
			process.waitFor();
			
			return output.toString();
		} catch (IOException e) {
			throw new RuntimeException(e);
		} catch (InterruptedException e) {
			throw new RuntimeException(e);
		}
	}

	public String getIpInfo() {
		return execNativeCommand(GetIpInfoCmd);
	}
	
	public String getValue(String name) {
		return execNativeCommand(GetPropCmd+" "+name);
	}
	
	public void setValue(String key, String name) {
		execNativeCommand(SetPropCmd+" "+key+" "+name);
	}
	
	public void removeValue(String key) {
		execNativeCommand(RmPropCmd+" "+key+" \"\"");
	}
	
}
