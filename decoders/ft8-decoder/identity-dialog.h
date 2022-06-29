
#ifndef	__IDENTITY_SELECTOR__
#define	__IDENTITY_SELECTOR__

#include	<QDialog>
#include	<QDialogButtonBox>
#include	<QMessageBox>
#include	<QLabel>
#include	<QLineEdit>
#include	<QGridLayout>
#include	<QSettings>

class	identityDialog: public QDialog {
public:
		identityDialog	(QSettings *, QWidget *parent = nullptr);
		~identityDialog	();
private slots:
	QSettings	*ft8Settings;
	void		verify	();
	void		reject	();

	QLabel		*callsignLabel;
	QLineEdit	*callsign;
	QLabel		*gridLabel;
	QLineEdit	*homeGrid;
	QLabel		*antennaLabel;
	QLineEdit	*antenna;
	QDialogButtonBox *buttonBox;

};
#endif

