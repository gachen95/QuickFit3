//19.5,19.6,19.7,19.8,19.9,20.0,20.1,20.2,20.3,20.4,20.5,20.6,20.7,20.8,20.9,21.0,21.1,21.2,21.3,21.4,21.6,21.8,22,22.2,22.4,22.6,22.8,23,23.2,23.4,23.6,23.8,24,24.2,24.4,24.6,24.8,25
//params_VQ=new Array(19.5,19.6,19.7,19.8,19.9,20.0,20.1,20.2,20.3,20.4,20.5,20.6,20.7,20.8,20.9,21.0,21.1,21.2,21.3,21.4,21.6,21.8,22,22.2,22.4,22.6,22.8,23,23.2,23.4,23.6,23.8,24,24.2,24.4,24.6,24.8,25);
params_VQ=new Array(0.5,0.6,0.7,0.8,0.9,1,1.1,1.2,1.3);
params_led_brightness=new Array(30,10,5,2,1,0);
for (var j=0; j<params_VQ.length; j++) {
  tools.showMessageBox("SET VQ to " + params_VQ[j]);
  for (var i=0; i<params_led_brightness.length; i++) {
    acquisition.clearAcquisitionCamera2Settings();
    instrument.setBrightfieldIntensity(0, 0, params_led_brightness[i]);
    tools.sleepMS(500);
    instrument.setAcqDescCurrentSample("C21_LED"+params_led_brightness[i]+"mA_24000mVOP_"+ params_VQ[j]*1000+"mVQ_");
    acquisition.acquirePreviewFrames(1,5, "%prefix%_chspad_%counter%");
    acquisition.acquirePreviewFrames(0, 3, "%prefix%_emccd_%counter%");
  }  
}