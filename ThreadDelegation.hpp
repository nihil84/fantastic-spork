class IDelegator : public QObject {
	Q_OBJECT
public:
	IDelegator() {
		connect(this, SIGNAL(invoke()), SLOT(onInvoke()));
	}
signals:
	void invoke();
private slots:
	virtual void onInvokation() = 0;
};