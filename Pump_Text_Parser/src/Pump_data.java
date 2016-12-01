
public class Pump_data {
	private String YearMonthDate ;
	private String HourMinuteSecond ;
	private float flowrate;
	private int counter;
	
	public String getYearMonthDate() {
        return YearMonthDate;
    }
	public String getHourMinuteSecond(){
		
		return HourMinuteSecond;
		
	}
	public float getflowrate(){
		return flowrate;
	}
	public int getcounter(){
		
		return counter;
	}
	
	public Pump_data(String YearMonthDate, String HourMinuteSecond, float flowrate, int counter){
		this.YearMonthDate = YearMonthDate;
		this.HourMinuteSecond = HourMinuteSecond;
		this.flowrate = flowrate;
		this.counter = counter; 
	}
	
	
}
