package com.abc;

import java.io.UnsupportedEncodingException;
import java.util.concurrent.ConcurrentLinkedQueue;

import android.annotation.SuppressLint;
import android.app.AlertDialog;
import android.app.NativeActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnCancelListener;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.text.InputType;
import android.util.Log;
import android.view.KeyEvent;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;
@SuppressLint("NewApi")
public class GameActivity extends NativeActivity {

	final static String dataVersion = "1.1";
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		CONTEXT = this;
		sSdcardPath = Environment.getExternalStorageDirectory() + java.io.File.separator;
	}

	public static String getExternalPathJni()
	{
		return sSdcardPath;
	}
	
	static GameActivity CONTEXT = null;
	static String sSdcardPath = null;
	static EditBoxMessage sMsg = null;
	static int MSG_EDITBOX = 1;
	static Handler sHandler = new Handler()
	{
		public void handleMessage(android.os.Message msg) 
		{
			if (msg.what == MSG_EDITBOX)
			{
				CONTEXT.showEditBoxDialog();
			}
		};
	};
	
	public static void showEditTextDialog(
		final String title,String content,int mode,int flag,int retype,int len)
	{
		sMsg = new EditBoxMessage(title, content, mode, flag, retype, len);
		sHandler.sendEmptyMessage(MSG_EDITBOX);
	}
	public static class EditBoxMessage {
		public String title;
		public String content;
		public int inputMode;
		public int inputFlag;
		public int returnType;
		public int maxLength;
		
		public EditBoxMessage(String title, String content, int inputMode, int inputFlag, int returnType, int maxLength)
		{
			this.content = content;
			this.title = title;
			this.inputMode = inputMode;
			this.inputFlag = inputFlag;
			this.returnType = returnType;
			this.maxLength = maxLength;
		}
	}
	public native static void nativeSetEditBoxResult(final byte[] pBytes,boolean isCancel);
	
	
	private static ConcurrentLinkedQueue<Runnable> jobs = new ConcurrentLinkedQueue<Runnable>();
	public static void dispatchPendingRunnables() 
	{
		if (jobs.size() <= 0) return;
		for (int i = 4; i > 0; i--) {
			Runnable job = jobs.poll();
			if (job == null) {
				return;
			}
			job.run();
		}
	}

//	public static void runOnGLThread(final Runnable r) {
//		jobs.add(r);
//	}
//	
	public static void setEditBoxResult(final String ret,final boolean cancel)
	{
		try {
			final byte[] b = ret.getBytes("UTF8");
			
			jobs.add(new Runnable()
			{
				@Override
				public void run() {
					// TODO Auto-generated method stub
					nativeSetEditBoxResult(b,cancel);
				}
			});
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
		
	static{
		System.loadLibrary("Mine");
	}
	public void showEditBoxDialog()
	{
		EditBoxMessage editBoxMessage = sMsg;
		final EditText edit = new EditText(this);
		edit.setText(editBoxMessage.content);
		edit.setSingleLine(true);
		if (editBoxMessage.inputFlag == 0)
		{
			edit.setInputType(InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_VARIATION_PASSWORD);
		}
		else
		{
			edit.setInputType(InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS);
		}
		edit.setOnEditorActionListener(new OnEditorActionListener() {
			@Override
			public boolean onEditorAction(final TextView v, final int actionId, final KeyEvent event) {
				if (actionId != EditorInfo.IME_NULL || (actionId == EditorInfo.IME_NULL && event != null && 
						event.getAction() == KeyEvent.ACTION_DOWN)) {
					
					final InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
					imm.hideSoftInputFromWindow(edit.getWindowToken(), 0);
					return true;
				}
				return false;
			}
		});
		AlertDialog dialog = new AlertDialog.Builder(this)
//			.setTitle(editBoxMessage.title)
			.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
				
				@Override
				public void onClick(DialogInterface dialog, int which) 
				{
					Log.e("ddd", "ok" + edit.getText().toString());
					//IrrlichtHelper.setEditTextDialogResult(edit.getText().toString(),false);
					setEditBoxResult(edit.getText().toString(),false);
				}
			})
			.setView(edit)
			.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
				
				@Override
				public void onClick(DialogInterface dialog, int which) {
					Log.e("ddd", "cancel" + edit.getText().toString());
					//IrrlichtHelper.setEditTextDialogResult(edit.getText().toString(),true);
					setEditBoxResult(edit.getText().toString(),true);
				}
			})
			.setOnCancelListener(new OnCancelListener() 
			{
				@Override
				public void onCancel(DialogInterface arg0) {
					// TODO Auto-generated method stub
					Log.e("ddd", "back cancel" + edit.getText().toString());
					//IrrlichtHelper.setEditTextDialogResult(edit.getText().toString(),true);
					setEditBoxResult(edit.getText().toString(),true);
				}
			})
			.setCancelable(false).show();
		
		    
			final Handler initHandler = new Handler();
			initHandler.postDelayed(new Runnable() {
				@Override
				public void run() {
					edit.requestFocus();
					edit.setSelection(edit.length());
					
					final InputMethodManager imm = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
					imm.showSoftInput(edit, 0);
				}
			}, 200);
	}
}
