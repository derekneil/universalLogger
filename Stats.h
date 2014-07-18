  //left labels
  tft.setCursor(12, TXTROW1);
  tft.print("2hr avg: ");
  tft.print("____");
  tft.setCursor(0, TXTROW2);
  tft.print("avg: ");
  tft.setCursor(0, TXTROW3);
  tft.print("max: ");

  //center labels
  tft.setCursor(110, TXTROW1);
  tft.print("cycles: ");

  tft.setCursor(104, TXTROW3);
  tft.print("encoder: ");
  tft.print("_____");

  //right labels
  tft.setCursor(220, TXTROW1);
  tft.print("2min avg: ");
  tft.print("____");
  tft.setCursor(228, TXTROW2);
  tft.print("last peak: ");
  tft.setCursor(240, TXTROW3);
  tft.print("force: ");

class Stats {
	
}: