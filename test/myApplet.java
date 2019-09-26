package test;

import java.applet.Applet;

public class MyApplet extends Applet {

  public void init() {
      super.init();
      System.out.println("init something");
  }

  public void jsCall(String hello) {
      System.out.println("this method is called by a js function and say :"
              + hello);
  }

}
