/*===============================================================================
Copyright (c) 2016 PTC Inc. All Rights Reserved.


Copyright (c) 2012-2014 Qualcomm Connected Experiences, Inc. All Rights Reserved.

Vuforia is a trademark of PTC Inc., registered in the United States and other 
countries.
===============================================================================*/

package com.eegeo.mobilesdkharness;

import com.vuforia.Vuforia;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;

public class ARModule
{
	private BackgroundThreadActivity m_backgroundActivity;
    static final int HIDE_LOADING_DIALOG = 0;
    static final int SHOW_LOADING_DIALOG = 1;
    
    private Object m_shutdownLock = new Object();
    
    private int m_vuforiaFlags = 0;
    
    int m_currentCamera = 0;
    
    private InitVuforiaTask m_initVuforiaTask;
    private LoadTrackerTask m_loadTrackerTask;
    
    /**
     * Creates a handler to update the status of the Loading Dialog from an UI
     * Thread
     */
    static class LoadingDialogHandler extends Handler
    {
        public void handleMessage(Message msg)
        { 
            if (msg.what == SHOW_LOADING_DIALOG)
            {
                BackgroundThreadActivity.m_loadingDialogContainer
                    .setVisibility(View.VISIBLE);
                
            } else if (msg.what == HIDE_LOADING_DIALOG)
            {
            	BackgroundThreadActivity.m_loadingDialogContainer.setVisibility(View.GONE);
            }
        }
    }
	private Handler loadingDialogHandler = new LoadingDialogHandler();
    
    /** An async task to initialize Vuforia asynchronously. */
    private class InitVuforiaTask extends AsyncTask<Void, Integer, Boolean>
    {
        // Initialize with invalid value:
        private int mProgressValue = -1;
        
        
        protected Boolean doInBackground(Void... params)
        {
            // Prevent the onDestroy() method to overlap with initialization:
            synchronized (m_shutdownLock)
            {
                Vuforia.setInitParameters(m_backgroundActivity, m_vuforiaFlags, "AQJPDKr/////AAAAGddZNNN7v0bggrFUaxqVUVN0n+1qqW3B9qOH7woVAjB2h/lc23NU8Oh+1dt4qyuelrh3M+55/jWcazJEoYEXbNqSlZeaxV6jGgggEb5qr6kIUK8ua8Ve8yGpi8rO5M7F0EwH4c1n8kL1avrBfWwo50zEs305oHpRU50HdyoGf9lD/jwOyw8uVOWcPc0tXalTgOsLoHN63HgOT1q+Rp+5zfec0RzCEZTCAGLUqYNmg+N3wUeULwC5EbKBKAn8NMJJZRspIrePuJE6fBWYWJKXTQLDCj/fRrCPg6afYipLi3Nr3bbyrrriMCkBuVvjS7NmBujEpWJuYL2roTaZ62VpIEE7wpmaACUTOLrwnFM58Hov");
                
                do
                {
                    // Vuforia.init() blocks until an initialization step is
                    // complete, then it proceeds to the next step and reports
                    // progress in percents (0 ... 100%).
                    // If Vuforia.init() returns -1, it indicates an error.
                    // Initialization is done when progress has reached 100%.
                    mProgressValue = Vuforia.init();
                    
                    // Publish the progress value:
                    publishProgress(mProgressValue);
                    
                    // We check whether the task has been canceled in the
                    // meantime (by calling AsyncTask.cancel(true)).
                    // and bail out if it has, thus stopping this thread.
                    // This is necessary as the AsyncTask will run to completion
                    // regardless of the status of the component that
                    // started is.
                } while (!isCancelled() && mProgressValue >= 0
                    && mProgressValue < 100);
                
                return (mProgressValue > 0);
            }
        }
        
        
        protected void onProgressUpdate(Integer... values)
        {
            // Do something with the progress value "values[0]", e.g. update
            // splash screen, progress bar, etc.
        }
        
        
        protected void onPostExecute(Boolean result)
        {
            // Done initializing Vuforia, proceed to next application
            // initialization status:
            if (result)
            {
            	Log.e("Eegeo-AR","InitVuforiaTask::onPostExecute: Vuforia "
                    + "initialization successful");
                
                //updateApplicationStatus(APPSTATUS_INIT_TRACKER);
            	initTracker();
            } else
            {
                // Create dialog box for display error:
                AlertDialog dialogError = new AlertDialog.Builder(
                    m_backgroundActivity).create();
                
                dialogError.setButton(DialogInterface.BUTTON_POSITIVE, "OK",
                    new DialogInterface.OnClickListener()
                    {
                        public void onClick(DialogInterface dialog, int which)
                        {
                            // Exiting application:
                            System.exit(1);
                        }
                    });
                // Show dialog box with error message:
                dialogError.setMessage("Log error");
                dialogError.show();
            }
        }
    }
    
    /** An async task to load the tracker data asynchronously. */
    private class LoadTrackerTask extends AsyncTask<Void, Integer, Boolean>
    {
        protected Boolean doInBackground(Void... params)
        {
            // Prevent the onDestroy() method to overlap:
            synchronized (m_shutdownLock)
            {
                // Load the tracker data set:
                return (NativeJniCalls.loadTrackerData() > 0);
            }
        }
        
        
        protected void onPostExecute(Boolean result)
        {
        	Log.e("Eegeo-AR","LoadTrackerTask::onPostExecute: execution "
                + (result ? "successful" : "failed"));
            
            if (result)
            {
                // Done loading the tracker, update application status:
                vuforiaInited();
                loadingDialogHandler.sendEmptyMessage(HIDE_LOADING_DIALOG);
            } else
            {
                // Create dialog box for display error:
                AlertDialog dialogError = new AlertDialog.Builder(
                    m_backgroundActivity).create();
                
                dialogError.setButton(DialogInterface.BUTTON_POSITIVE, "Close",
                    new DialogInterface.OnClickListener()
                    {
                        public void onClick(DialogInterface dialog, int which)
                        {
                            // Exiting application:
                            System.exit(1);
                        }
                    });
                
                // Show dialog box with error message:
                dialogError.setMessage("Failed to load tracker data.");
                dialogError.show();
            }
        }
    }
    
    public ARModule(BackgroundThreadActivity backgroundActivity)
	{
    	m_backgroundActivity = backgroundActivity;
    	m_vuforiaFlags = Vuforia.GL_20;
	}
    
    public void destroy()
    {
        if (m_initVuforiaTask != null
            && m_initVuforiaTask.getStatus() != InitVuforiaTask.Status.FINISHED)
        {
            m_initVuforiaTask.cancel(true);
            m_initVuforiaTask = null;
        }
        
        if (m_loadTrackerTask != null
            && m_loadTrackerTask.getStatus() != LoadTrackerTask.Status.FINISHED)
        {
            m_loadTrackerTask.cancel(true);
            m_loadTrackerTask = null;
        }
    }
    
    /**
     * NOTE: this method is synchronized because of a potential concurrent
     * access by onResume() and InitVuforiaTask.onPostExecute().
     */
    private synchronized void vuforiaInited()
    {
    	// NOTE: This is only a hint. There is no guarantee that the
    	// garbage collector will actually be run.
    	System.gc();
    	// Native post initialization:
    	NativeJniCalls.onVuforiaInitializedNative();
    	// Start the camera:
    }
    
	public void initVuforia()
	{
		loadingDialogHandler.sendEmptyMessage(SHOW_LOADING_DIALOG);
		 try
         {
             m_initVuforiaTask = new InitVuforiaTask();
             m_initVuforiaTask.execute();
         } catch (Exception e)
         {
             Log.e("Eegeo-AR","Initializing Vuforia SDK failed");
         }
	}
	
	private void initTracker()
	{
		 if (NativeJniCalls.initTracker() > 0)
         {
			loadTracker();
         }
	}
	
	private void loadTracker()
	{
		try
        {
            m_loadTrackerTask = new LoadTrackerTask();
            m_loadTrackerTask.execute();
        } catch (Exception e)
        {
        	 Log.e("Eegeo-AR","Loading tracking data set failed");
        }
	}

	public void deInitVuforia()
	{
        // Ensure that all asynchronous operations to initialize Vuforia
        // and loading the tracker datasets do not overlap:
        synchronized (m_shutdownLock)
        {
            // De-initialize Vuforia SDK:
            Vuforia.deinit();
        }
        
        System.gc();
	}

}
