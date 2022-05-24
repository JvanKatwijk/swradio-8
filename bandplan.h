#
#ifndef	__BANDPLAN__
#define	__BANDPLAN__

#include	<stdint.h>
#include        <QString>
#include        <QtXml>
#include        <QFile>
#include        <vector>
#include        <QStringList>

typedef struct {
	int     low;
	int     high;
	QString	label;
} bandElement;

class	bandPlan {
public:
	bandPlan	(QString);
	~bandPlan	();
const
QString	getFrequencyLabel	(uint32_t);
private:
	QString		fileName;
	std::vector<bandElement> labelTable;
	bool	loadPlan	(QString);
};
#endif
