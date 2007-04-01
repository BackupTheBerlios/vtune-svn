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
	QComboBox *typeBox;
public slots:
	void RepaintSpectrum(vtune_data *data);
	void ChangeType(int index);
signals:
	void spectrumChanged(vtune_data *data);
	void trackerTypeChanged(int index);
	
};

