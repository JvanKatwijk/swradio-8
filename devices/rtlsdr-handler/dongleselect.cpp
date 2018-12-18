#
#include	"dongleselect.h"
#include	<stdio.h>
#include	<QVBoxLayout>

	dongleSelect::dongleSelect (void) {
	toptext	= new QLabel (this);
	toptext	-> setText ("Select a dongle");
	selectorDisplay	= new QListView (this);
	QVBoxLayout	*layOut = new QVBoxLayout;
	layOut	-> addWidget (selectorDisplay);
	layOut	-> addWidget (toptext);
	setWindowTitle (tr("dongle select"));
	setLayout (layOut);

	dongleList. setStringList (Dongles);
	Dongles = QStringList ();
	dongleList. setStringList (Dongles);
	selectorDisplay	-> setModel (&dongleList);
	connect (selectorDisplay, SIGNAL (clicked (QModelIndex)),
	         this, SLOT (selectDongle (QModelIndex)));
	selectedItem	= -1;
}

	dongleSelect::~dongleSelect (void) {
}

void	dongleSelect::addtoDongleList (const char *v) {
QString s (v);

	Dongles << s;
	dongleList. setStringList (Dongles);
	selectorDisplay	-> setModel (&dongleList);
	selectorDisplay	-> adjustSize ();
	adjustSize ();
}

void	dongleSelect::selectDongle (QModelIndex s) {
	fprintf (stderr, "we have chosen %d\n", s.row ());
	selectedItem = s. row ();
	QDialog::done (s. row ());
//	close ();
}

int16_t	dongleSelect::getSelectedItem	(void) {
	return selectedItem;
}

