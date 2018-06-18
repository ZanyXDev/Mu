function useFramebuffer(){
   var buffer = [0];
   var color = toRgb16(0xFF69B4);//solid pink
   for(var index = 0; index < 100 * 100; index++)
      buffer[index] = color;
   jsSystem.setFramebufferSize(100, 100);
   jsSystem.setFramebuffer(buffer);
}

function main(args){
   userIo.writeStringJs("Test Input Required");
   while(!userIo.stringAvailableJs())
      jsSystem.uSleep(16666);
   var inputTest = userIo.readStringJs();
   userIo.writeStringJs("Test Input Received");
   if(typeof irdaCommands == 'undefined'){
      userIo.writeStringJs("Dependency blob is missing!");
   }
   else{
      useFramebuffer();
      var num = irdaCommands.IRDA_COMMAND_GET_BYTE;
      userIo.writeStringJs(inputTest + num.toString());
   }
}