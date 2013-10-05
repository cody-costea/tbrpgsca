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
import com.tbrpgsca.library.DataApp;
import com.tbrpgsca.library.R;

public class DemoAct extends Activity {
	DataApp data;	
	EditText name[]=new EditText[3];
	Spinner jobBox[]=new Spinner[3],raceBox[]=new Spinner[3];
	ArrayAdapter<String> jobList, raceList;
	Button Begin,About,Exit;
	int level=0;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_demo);
		data=(DataApp)this.getApplication();
		raceList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		jobList=new ArrayAdapter<String>(this,android.R.layout.simple_spinner_item);
		raceBox[0]=(Spinner)findViewById(R.id.RaceBox1);
		raceBox[1]=(Spinner)findViewById(R.id.RaceBox2);
		raceBox[2]=(Spinner)findViewById(R.id.RaceBox3);
		for (int i=0;i<data.pcRace.length;i++) raceList.add(data.pcRace[i].rname);
		for (int i=0;i<3;i++) raceBox[i].setAdapter(raceList);
		raceBox[0].setSelection(2);
		raceBox[1].setSelection(2);
		raceBox[2].setSelection(2);
		jobBox[0]=(Spinner)findViewById(R.id.JobBox1);
		jobBox[1]=(Spinner)findViewById(R.id.JobBox2);
		jobBox[2]=(Spinner)findViewById(R.id.JobBox3);
		for (int i=0;i<data.pcJob.length;i++) jobList.add(data.pcJob[i].jname);
		for (int i=0;i<3;i++) jobBox[i].setAdapter(jobList);
		jobBox[0].setSelection(7);
		jobBox[1].setSelection(4);
		jobBox[2].setSelection(2);
		name[0]=(EditText)findViewById(R.id.NameField1);
		name[1]=(EditText)findViewById(R.id.NameField2);
		name[2]=(EditText)findViewById(R.id.NameField3);
		Begin=(Button)findViewById(R.id.StartBt);
		About=(Button)findViewById(R.id.InfoBt);
		Exit=(Button)findViewById(R.id.QuitBt);
		Begin.setOnClickListener(cAction);
		About.setOnClickListener(cAction);
		Exit.setOnClickListener(cAction);
		data.party[0]=1;data.party[1]=2;data.party[2]=3;
		for (int i=0;i<data.Item.length;i++)
			data.Item[i].qty=5;		
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
				n="Claudius";
				break;
			case 2:
				n="George";
				break;
			case 3:
				n="Stephen";
				break;
			}
		data.Player[p].name=n;
		if (raceBox[p-1].isEnabled())
			data.Player[p].setRace(data.pcRace[raceBox[p-1].getSelectedItemPosition()], true);
		for (int i=0;i<data.Player[p].skill.size();i++)
			if (data.Skill[data.Player[p].skill.get(i)].lvrq>4) data.Player[p].skill.remove(i);
		data.Player[p].changeJob(data.pcJob[jobBox[p-1].getSelectedItemPosition()], false);
		raceBox[p-1].setEnabled(false);
	}
	
	private void beginBt(){
		for (int i=1;i<=3;i++) setPlayer(i);
		Intent btInt;
		btInt = new Intent(this, BattleAct.class);
		if (level%2==1){
			btInt.putExtra("Surprise", true);		
			btInt.putExtra("Escape", false);
		}
		btInt.putExtra("Enemy", level);
		startActivityForResult(btInt, 1);
		for (int i=1;i<data.Player.length;i++) data.Player[i].recover();
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