class CO3{
	private:
		char c_buffer[8], f_buffer[8];
		int cov, co2;
		int err;
		int scaled_ethanol_signal, scaled_h2_signal;
		int c_temp;

	public:
		void start();
		void update();

		char getTemperature();
		int get_temperature();
		int getCOV(){ return cov; }
		int getCO2(){ return co2; }

		void blink(bool state);
		void bip(bool state);

		void writeLCD(char text, int line, int column);
		void setLCDColor(int R, int G, int B);
};