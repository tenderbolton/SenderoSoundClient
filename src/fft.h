
#ifndef _FFT
#define _FFT

#ifndef M_PI
#define	M_PI		3.14159265358979323846  /* pi */
#endif


class fft {
	
	public:
		
	fft();
	~fft();	
	
	/* Calculate the power spectrum */
	void powerSpectrum (float *data, int windowSize, float *magnitude);

};


#endif	
