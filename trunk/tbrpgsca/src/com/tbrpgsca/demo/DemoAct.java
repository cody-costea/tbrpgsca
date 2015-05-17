package com.tbrpgsca.demo;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.os.Bundle;
import android.text.SpannableString;
import android.text.util.Linkify;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;

import com.tbrpgsca.library.BattleAct;
import com.tbrpgsca.library.AppData;
import com.tbrpgsca.library.R;

public class DemoAct extends Activity {
	//DataApp data;	
	EditText name[]=new EditText[3];
	Spinner jobBox[]=new Spinner[3],raceBox[]=new Spinner[3];
	ArrayAdapter<String> jobList, raceList;
	Button Begin,About,Exit;
	int level=0;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_demo);
		//data=(DataApp)this.getApplication();
		raceList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		jobList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		raceBox[0]=(Spinner)findViewById(R.id.RaceBox1);
		raceBox[1]=(Spinner)findViewById(R.id.RaceBox2);
		raceBox[2]=(Spinner)findViewById(R.id.RaceBox3);
		for (int i=0;i<AppData.pcRace.length;i++) raceList.add(AppData.pcRace[i].rname);
		for (int i=0;i<3;i++) raceBox[i].setAdapter(raceList);
		raceBox[0].setSelection(1);
		raceBox[1].setSelection(0);
		raceBox[2].setSelection(2);
		jobBox[0]=(Spinner)findViewById(R.id.JobBox1);
		jobBox[1]=(Spinner)findViewById(R.id.JobBox2);
		jobBox[2]=(Spinner)findViewById(R.id.JobBox3);
		for (int i=0;i<AppData.pcJob.length;i++) jobList.add(AppData.pcJob[i].jname);
		for (int i=0;i<3;i++) jobBox[i].setAdapter(jobList);
		jobBox[0].setSelection(0);
		jobBox[1].setSelection(5);
		jobBox[2].setSelection(3);
		name[0]=(EditText)findViewById(R.id.NameField1);
		name[1]=(EditText)findViewById(R.id.NameField2);
		name[2]=(EditText)findViewById(R.id.NameField3);
		Begin=(Button)findViewById(R.id.StartBt);
		About=(Button)findViewById(R.id.InfoBt);
		Exit=(Button)findViewById(R.id.QuitBt);
		Begin.setOnClickListener(cAction);
		About.setOnClickListener(cAction);
		Exit.setOnClickListener(cAction);
		AppData.party[0]=1;AppData.party[1]=2;AppData.party[2]=3;AppData.party[3]=0;
		for (int i=0;i<AppData.Item.length;i++)
			AppData.Item[i].qty=5;		
	}
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.battle, menu);
		return true;
	}
	
	private void setPlayer(int p){
		String n=name[p-1].getText().toString();
		if (n.equals(""))
			switch (p){
			case 1:
				n="Cody";
				break;
			case 2:
				n="George";
				break;
			case 3:
				n="Stephen";
				break;
			}
		AppData.Player[p].name=n;
		if (raceBox[p-1].isEnabled())
			AppData.Player[p].setRace(AppData.pcRace[raceBox[p-1].getSelectedItemPosition()], true);		
		AppData.Player[p].changeJob(AppData.pcJob[jobBox[p-1].getSelectedItemPosition()], false);
		raceBox[p-1].setEnabled(false);
		//DataApp.Player[1].setSprName("Knight");DataApp.Player[2].setSprName("Dragoon");DataApp.Player[3].setSprName("Templar");
	}
	
	private void beginBt(){
		for (int i=1;i<=3;i++) setPlayer(i);
		Intent btInt;
		btInt = new Intent(this, BattleAct.class);
		if (level%2==1){
			btInt.putExtra("Surprise", -1);		
			btInt.putExtra("Escape", false);
		}
		if (level==2) btInt.putExtra("Surprise", 1);
		btInt.putExtra("Enemy", level);
		startActivityForResult(btInt, 1);
		for (int i=1;i<AppData.Player.length;i++) AppData.Player[i].recover();
	}
	
	private OnClickListener cAction = new OnClickListener() {
    	@Override
    	public void onClick(final View v){
    		switch (v.getId()){
    			case R.id.StartBt:
    				beginBt();
    			break;
    			case R.id.InfoBt:
    				displayMsg("About", getString(R.string.msg_about));
    			break;
    			case R.id.QuitBt:
    				finish();
    			break;
    		}
    	}
    };
    
    private void displayMsg(String t,String s){
    	SpannableString text=new SpannableString(s);
    	Linkify.addLinks(text, Linkify.WEB_URLS);
    	AlertDialog.Builder msg = new AlertDialog.Builder(this);
    	msg.setCancelable(false);
    	msg.setMessage(text);
    	msg.setTitle(t);
    	msg.setPositiveButton("Ok", new DialogInterface.OnClickListener() {			
			@Override
			public void onClick(DialogInterface arg0, int arg1) {
				// TODO Auto-generated method stub
			}
		});
    	AlertDialog dialog = msg.create();
    	dialog.show();
    }
    
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        if (requestCode >= 0) {
            if (resultCode == RESULT_OK) {
            	int result=data.getExtras().getInt("Outcome");
        		if (result>0&&level<5) level++;
            }
        }
    }
}