#ifndef	__FAX_BUFFER_H
#define	__FAX_BUFFER_H


class	buffer {
public:
		buffer			(int, int, int);
		~buffer			();
	void	setMode			(int);
	void	add			(int);
	void	prepare_recognize_start	();
	bool	startSignal_Detected	();
	void	prepare_recognize_phaseLine	();
	bool	phaseLine_recognized	();
	void	prepare_read_phaseLine	();
	void	phaseLine_read		();
	void	prepare_read_imageLine	();
	bool	imageLine_Read		();
	void	prepare_recognize_stop	();
	bool	stopSignal_Detected	();
	bool	wait_too_long		(int);

private:

	bool	checkWite		(int, int, int);
	bool	checkBlack		(int, int, int);
	int	samplesperLine;
	int	startFrequency;
	int	stopFrequency;
	int	linesperImage;

	std::vector<int>	theBuffer;
	int	nrItems;
	int	bufferP;
	int	deadLine;
};
#endif

	
