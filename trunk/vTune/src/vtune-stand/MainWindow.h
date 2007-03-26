class vSpectrum;

class vMainWindow : public QWidget
{
	Q_OBJECT

public:
	vMainWindow(QWidget *Parent = 0);
private:
	void Close();
	static void callback(vtune_data *data);
	vTune *vtune;
	vSpectrum *spectrum;
	QLabel *freqText;
	QLabel *freqMag;
	QSplitter *splitter;
private slots:
	void RepaintSpectrum(vtune_data *data);
signals:
	void spectrumChanged(vtune_data *data);

};

