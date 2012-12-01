package com.androVM.vmconfig;

import java.util.Scanner;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.PowerManager;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.CheckBox;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class androVMConfig extends Activity {
    /** Called when the activity is first created. */
	private androVMTextConfig TextConfig;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);

    	CheckBox cb_opengl = (CheckBox) findViewById(R.id.cb_opengl);
    	CheckBox cb_res = (CheckBox) findViewById(R.id.cb_res);
        TextView tv_res = (TextView) findViewById(R.id.tv_res);
    	Spinner sp_res = (Spinner) findViewById(R.id.sp_res);
    	CheckBox cb_dpi = (CheckBox) findViewById(R.id.cb_dpi);
        TextView tv_dpi = (TextView) findViewById(R.id.tv_dpi);
    	Spinner sp_dpi = (Spinner) findViewById(R.id.sp_dpi);
        CheckBox cb_opengl_disable_render = (CheckBox) findViewById(R.id.cb_opengl_disable_render);
        TextView tv_opengl_disable_render = (TextView) findViewById(R.id.tv_opengl_disable_render);
        CheckBox cb_opengl_force_IP = (CheckBox) findViewById(R.id.cb_opengl_force_IP);
        TextView tv_opengl_force_IP = (TextView) findViewById(R.id.tv_opengl_force_IP);
        EditText et_opengl_force_IP = (EditText) findViewById(R.id.et_opengl_force_IP);
    	CheckBox cb_keyboard = (CheckBox) findViewById(R.id.cb_keyboard);
    	Spinner sp_keyboard = (Spinner) findViewById(R.id.sp_keyboard);
    	CheckBox cb_statusbar = (CheckBox) findViewById(R.id.cb_statusbar);
    	CheckBox cb_su_bypass = (CheckBox) findViewById(R.id.cb_su_bypass);
    	 
    	TextConfig = new androVMTextConfig();

	String ip_info = TextConfig.getIpInfo();
	TextView tv_ipinfo = (TextView) findViewById(R.id.tv_infoip);
	tv_ipinfo.setText(ip_info);
       
        String cfg_opengl = TextConfig.getValue("hardware_opengl");
        String cfg_res = TextConfig.getValue("vbox_graph_mode");
        String cfg_dpi = TextConfig.getValue("vbox_dpi");
        String cfg_opengl_disable_render = TextConfig.getValue("hardware_opengl_disable_render");
        String cfg_opengl_force_IP = TextConfig.getValue("hardware_opengl_force_IP");
        String cfg_keyboard = TextConfig.getValue("keyboard_disable");
	String cfg_statusbar = TextConfig.getValue("statusbar_present");
	String cfg_su_bypass = TextConfig.getValue("su_bypass");

	String[] listRes = getResources().getStringArray(R.array.list_resolutions);
        ArrayAdapter<String> res_adapter = new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item,listRes);
        res_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        sp_res.setAdapter(res_adapter);
        sp_res.setEnabled(false);

        if ((cfg_opengl != null) && (cfg_opengl.length()>0)) {
            cb_opengl.setChecked(true);
            cb_res.setVisibility(android.view.View.INVISIBLE);
            tv_res.setVisibility(android.view.View.INVISIBLE);
            sp_res.setVisibility(android.view.View.INVISIBLE);
            cb_dpi.setVisibility(android.view.View.INVISIBLE);
            tv_dpi.setVisibility(android.view.View.INVISIBLE);
            sp_dpi.setVisibility(android.view.View.INVISIBLE);
        }
        else { 
            cb_opengl_disable_render.setVisibility(android.view.View.INVISIBLE);
            tv_opengl_disable_render.setVisibility(android.view.View.INVISIBLE);
            cb_opengl_force_IP.setVisibility(android.view.View.INVISIBLE);
            tv_opengl_force_IP.setVisibility(android.view.View.INVISIBLE);
            et_opengl_force_IP.setVisibility(android.view.View.INVISIBLE);
        }
        cb_res.setChecked(false);

        if ((cfg_res != null) && (cfg_res.length()>0)) {        	        	
            int cpos = res_adapter.getPosition(cfg_res);
            if (cpos>=0) {
        	sp_res.setSelection(cpos);
        	sp_res.setEnabled(true);
        	cb_res.setChecked(true);
            }
        }
        
        cb_dpi.setChecked(false);
        String[] listDpi = { "120", "160", "213", "240", "320" };
        ArrayAdapter<String> dpi_adapter = new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item,listDpi);
        dpi_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        sp_dpi.setAdapter(dpi_adapter);
        sp_dpi.setEnabled(false);
        if ((cfg_dpi != null) && (cfg_dpi.length()>0)) {
        	int cpos = dpi_adapter.getPosition(cfg_dpi);
        	if (cpos>=0) {
        		sp_dpi.setSelection(cpos);
        		sp_dpi.setEnabled(true);
        		cb_dpi.setChecked(true);
        	}
        }

        if ((cfg_opengl_disable_render != null) && (cfg_opengl_disable_render.length()>0)) {
            cb_opengl_disable_render.setChecked(true);
        }

        et_opengl_force_IP.setEnabled(false);
        if ((cfg_opengl_force_IP != null) && (cfg_opengl_force_IP.length()>0)) {
            cb_opengl_force_IP.setChecked(true);
            et_opengl_force_IP.setEnabled(true);
            et_opengl_force_IP.setText(cfg_opengl_force_IP);
        }
            
        cb_keyboard.setChecked(false);
        String[] listKeyboards = { "Physical keyboard", "Virtual keyboard", "Virtual keyboard with physical hotkeys" };
        ArrayAdapter<String> keyboard_adapter = new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item,listKeyboards); 
        keyboard_adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        sp_keyboard.setAdapter(keyboard_adapter);       
        sp_keyboard.setEnabled(false);
        if ((cfg_keyboard != null) && (cfg_keyboard.length()>0)) {
        	try {
        		sp_keyboard.setSelection(Integer.parseInt(cfg_keyboard));
        		sp_keyboard.setEnabled(true);
        		cb_keyboard.setChecked(true);
        	} catch (NumberFormatException e) {
        	}
        }
        
        cb_statusbar.setChecked(false);
        if ((cfg_statusbar != null) && (cfg_statusbar.length()>0)) {
        	try {
        		cb_statusbar.setChecked(true);
        	} catch (NumberFormatException e) {
        	}
        }

        cb_su_bypass.setChecked(false);
        if ((cfg_su_bypass != null) && (cfg_su_bypass.length()>0)) {
        	try {
        		cb_su_bypass.setChecked(true);
        	} catch (NumberFormatException e) {
        	}
        }
    }
    
    public void onCBClick_opengl(View v) {
    	CheckBox cb_res = (CheckBox) findViewById(R.id.cb_res);
        TextView tv_res = (TextView) findViewById(R.id.tv_res);
    	Spinner sp_res = (Spinner) findViewById(R.id.sp_res);
    	CheckBox cb_dpi = (CheckBox) findViewById(R.id.cb_dpi);
        TextView tv_dpi = (TextView) findViewById(R.id.tv_dpi);
    	Spinner sp_dpi = (Spinner) findViewById(R.id.sp_dpi);
        CheckBox cb_opengl_disable_render = (CheckBox) findViewById(R.id.cb_opengl_disable_render);
        TextView tv_opengl_disable_render = (TextView) findViewById(R.id.tv_opengl_disable_render);
        CheckBox cb_opengl_force_IP = (CheckBox) findViewById(R.id.cb_opengl_force_IP);
        TextView tv_opengl_force_IP = (TextView) findViewById(R.id.tv_opengl_force_IP);
        EditText et_opengl_force_IP = (EditText) findViewById(R.id.et_opengl_force_IP);

        if (((CheckBox) v).isChecked()) {
            cb_res.setVisibility(android.view.View.INVISIBLE);
            tv_res.setVisibility(android.view.View.INVISIBLE);
            sp_res.setVisibility(android.view.View.INVISIBLE);
            cb_dpi.setVisibility(android.view.View.INVISIBLE);
            tv_dpi.setVisibility(android.view.View.INVISIBLE);
            sp_dpi.setVisibility(android.view.View.INVISIBLE);
            cb_opengl_disable_render.setVisibility(android.view.View.VISIBLE);
            tv_opengl_disable_render.setVisibility(android.view.View.VISIBLE);
            cb_opengl_force_IP.setVisibility(android.view.View.VISIBLE);
            tv_opengl_force_IP.setVisibility(android.view.View.VISIBLE);
            et_opengl_force_IP.setVisibility(android.view.View.VISIBLE);
        } else {
            cb_res.setVisibility(android.view.View.VISIBLE);
            tv_res.setVisibility(android.view.View.VISIBLE);
            sp_res.setVisibility(android.view.View.VISIBLE);
            cb_dpi.setVisibility(android.view.View.VISIBLE);
            tv_dpi.setVisibility(android.view.View.VISIBLE);
            sp_dpi.setVisibility(android.view.View.VISIBLE);
            cb_opengl_disable_render.setVisibility(android.view.View.INVISIBLE);
            tv_opengl_disable_render.setVisibility(android.view.View.INVISIBLE);
            cb_opengl_force_IP.setVisibility(android.view.View.INVISIBLE);
            tv_opengl_force_IP.setVisibility(android.view.View.INVISIBLE);
            et_opengl_force_IP.setVisibility(android.view.View.INVISIBLE);
        }
    }

    public void onCBClick_res(View v) {
    	Spinner sp_res = (Spinner) findViewById(R.id.sp_res);

        if (((CheckBox) v).isChecked()) {
        	sp_res.setEnabled(true);
        } else {
        	sp_res.setEnabled(false);
        }
    }
    
    public void onCBClick_dpi(View v) {
    	Spinner sp_dpi = (Spinner) findViewById(R.id.sp_dpi);
        if (((CheckBox) v).isChecked()) {
        	sp_dpi.setEnabled(true);
        } else {
        	sp_dpi.setEnabled(false);
        }
    }
    
    public void onCBClick_opengl_disable_render(View v) {
    }
    
    public void onCBClick_opengl_force_IP(View v) {
    	EditText et_opengl_force_IP = (EditText) findViewById(R.id.et_opengl_force_IP);
        if (((CheckBox) v).isChecked()) {
        	et_opengl_force_IP.setEnabled(true);
        } else {
        	et_opengl_force_IP.setEnabled(false);
        }
    }
    
    public void onCBClick_keyboard(View v) {
    	Spinner sp_keyboard = (Spinner) findViewById(R.id.sp_keyboard);
        if (((CheckBox) v).isChecked()) {
        	sp_keyboard.setEnabled(true);
        } else {
        	sp_keyboard.setEnabled(false);
        }
    }
    
    public void onCBClick_statusbar(View v) {
    }
    
    public void onBTSave(View v) {
    	// Hardware OpenGL
    	CheckBox cb_opengl = (CheckBox) findViewById(R.id.cb_opengl);
    	
    	if (cb_opengl.isChecked()) 
    	    TextConfig.setValue("hardware_opengl","1");
    	else
    	    TextConfig.removeValue("hardware_opengl");

    	// Resolution
    	CheckBox cb_res = (CheckBox) findViewById(R.id.cb_res);
    	
    	if (cb_res.isChecked()) {
    		Spinner sp_res = (Spinner) findViewById(R.id.sp_res);
    		String new_res = sp_res.getSelectedItem().toString();
    		
    		TextConfig.setValue("vbox_graph_mode",new_res);
    	}
    	else
    		TextConfig.removeValue("vbox_graph_mode");
    	
    	// DPI
    	CheckBox cb_dpi = (CheckBox) findViewById(R.id.cb_dpi);
    	
    	if (cb_dpi.isChecked()) {
    		Spinner sp_dpi = (Spinner) findViewById(R.id.sp_dpi);
    		String new_dpi = sp_dpi.getSelectedItem().toString();
    		
    		TextConfig.setValue("vbox_dpi",new_dpi);
    	}
    	else
    		TextConfig.removeValue("vbox_dpi");

        // OpenGL Disable Render
        CheckBox cb_opengl_disable_render = (CheckBox) findViewById(R.id.cb_opengl_disable_render);

        if (cb_opengl_disable_render.isChecked())
            TextConfig.setValue("hardware_opengl_disable_render", "1");
        else
            TextConfig.removeValue("hardware_opengl_disable_render");

        // OpenGL Force IP
        CheckBox cb_opengl_force_IP = (CheckBox) findViewById(R.id.cb_opengl_force_IP);

        if (cb_opengl_force_IP.isChecked()) {
            EditText et_opengl_force_IP = (EditText) findViewById(R.id.et_opengl_force_IP);
            TextConfig.setValue("hardware_opengl_force_IP", et_opengl_force_IP.getText().toString());
    	}
        else
            TextConfig.removeValue("hardware_opengl_force_IP");
    	
    	// Keyboard
    	CheckBox cb_keyboard = (CheckBox) findViewById(R.id.cb_keyboard);
    	
    	if (cb_keyboard.isChecked()) {
    		Spinner sp_keyboard = (Spinner) findViewById(R.id.sp_keyboard);
    		long keyboard_select = sp_keyboard.getSelectedItemId();
    		if (keyboard_select >= 0)
    			TextConfig.setValue("keyboard_disable", String.valueOf(keyboard_select));
    		else
    			TextConfig.removeValue("keyboard_disable");
    	}
    	else
    		TextConfig.removeValue("keyboard_disable");
    	
    	// Statusbar
    	CheckBox cb_statusbar = (CheckBox) findViewById(R.id.cb_statusbar);
    	
    	if (cb_statusbar.isChecked()) {
    		TextConfig.setValue("statusbar_present", "1");
    	}
    	else
    		TextConfig.removeValue("statusbar_present");
    	
    	// su bypass
    	CheckBox cb_su_bypass = (CheckBox) findViewById(R.id.cb_su_bypass);
    	
    	if (cb_su_bypass.isChecked()) {
    		TextConfig.setValue("su_bypass", "1");
    	}
    	else
    		TextConfig.removeValue("su_bypass");
    	
    	// Reboot ?
    	AlertDialog.Builder builder_reboot = new AlertDialog.Builder(this);
    	builder_reboot.setMessage("Do you want to reboot ?")
    		          .setCancelable(false)
    		          .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
    			         public void onClick(DialogInterface dialog, int id) {
    				        // Reboot    
    			        	PowerManager pm = (PowerManager) getSystemService(Context.POWER_SERVICE);
    			        	pm.reboot(POWER_SERVICE);    			        
    			        	// Finish activity
    			        	androVMConfig.this.finish();
    			         }
    		          })
    		          .setNegativeButton("No", new DialogInterface.OnClickListener() {
    			         public void onClick(DialogInterface dialog, int id) {
    			        	// Finish activity
    			        	androVMConfig.this.finish();
    			         }
    		          });    				   
    	
    	AlertDialog reboot_dlg = builder_reboot.create();
    	reboot_dlg.show();   
    }
}
