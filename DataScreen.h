

class Display {
	private:
		int active;
		SensorInput sensors[];

	public:
		Display(SensorInput sensors[]) :
		this->sensors = sensors;
		{
			active = 0;
		}

		int aquireADisplayRegion(SensorInput sensor){
			switch (active) {
				case >3:
					return false;
				case 0:
					//this will be the only sensor displayed
					x =
					y = 
					w = 
					h =
					break;
				case 1:
					// 
					break;
				case 2:
					// 
					break;
				case 3:
					// 
					break;
			}

			active++;

			//update 

			return true;
		}
}