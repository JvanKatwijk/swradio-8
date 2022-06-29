
#include	"identity-dialog.h"


identityDialog::identityDialog (QSettings *ft8Settings, QWidget *parent):
	                                        QDialog (parent) {
	this	-> ft8Settings	= ft8Settings;
	callsignLabel	= new QLabel (tr("callsign"));
	gridLabel	= new QLabel (tr("grid"));
	antennaLabel	= new QLabel (tr("antenna"));

	callsign	= new QLineEdit;
	homeGrid	= new QLineEdit;
	antenna		= new QLineEdit;

	buttonBox	= new QDialogButtonBox (QDialogButtonBox::Ok |
	                                        QDialogButtonBox::Cancel);
	connect (buttonBox, &QDialogButtonBox::accepted,
	                                this, &identityDialog::verify);
	connect (buttonBox, &QDialogButtonBox::rejected,
	                                this, &identityDialog::reject);
	QGridLayout *mainLayout = new QGridLayout;
	mainLayout	-> addWidget (callsignLabel, 0, 0);
	mainLayout	-> addWidget (callsign, 0, 1);
	mainLayout	-> addWidget (gridLabel, 1, 0);
	mainLayout	-> addWidget (homeGrid, 1, 1);
	mainLayout	-> addWidget (antennaLabel, 2, 0);
	mainLayout	-> addWidget (antenna, 2, 1);
	mainLayout	-> addWidget (buttonBox, 3, 0);
	setLayout (mainLayout);
	setWindowTitle ("your data");
	show ();;
}

identityDialog::~identityDialog	() {}

void	identityDialog::verify	() {
	if (!callsign -> text (). isEmpty () &&
	    !homeGrid -> text (). isEmpty () &&
	    !antenna -> text (). isEmpty ()) {
	   ft8Settings	-> beginGroup ("ft8Settings");
	   ft8Settings	-> setValue ("homeCall", callsign -> text ());
	   ft8Settings	-> setValue ("homeGrid", homeGrid -> text ());
	   ft8Settings	-> setValue ("antenna",  antenna -> text ());
	   ft8Settings	-> endGroup ();
	   accept ();
	   return;
	}

	QMessageBox::StandardButton answer;
	answer = QMessageBox::warning (this, tr ("incomplete form"),
	            tr ("The form is not filled in completely\n"
	                 "Do you want to discard it?"),
	                      QMessageBox::Yes |QMessageBox::No);
	if (answer == QMessageBox::Yes)
	   reject ();
}

void	identityDialog::reject	() {
	accept ();
}



	
	
	

