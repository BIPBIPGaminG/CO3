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
		int & getCOV()       { return cov; }
  const int & getCOV() const { return cov; }
        int & getCO2()       { return co2; }
  const int & getCO2() const { return co2; }

		void blink(const bool* state);
		void bip(const bool* state);

		void writeLCD(const char* text, const int* line, const int* column);
		void setLCDColor(const int* R, const int* G, const int* B);
};