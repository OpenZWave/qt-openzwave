#ifndef QTOZWREPLICA_H
#define QTOZWREPLICA_H

#include <QObject>
#include "qt-openzwave/qtozw_pods.h"

class QTOPENZWAVESHARED_EXPORT QTOZWReplicaBase : public QObject {
    Q_OBJECT
    public:
        QTOZWReplicaBase(ConnectionType::Type connectionType, QObject *parent);
        ~QTOZWReplicaBase();

        Q_PROPERTY(ConnectionType::Type connectionType MEMBER m_connectionType READ getConnectionType WRITE setConnectionType NOTIFY connectionTypeChanged);
        Q_PROPERTY(bool ready MEMBER m_ready READ isReady WRITE setReady NOTIFY readyChanged);

        ConnectionType::Type getConnectionType();
        bool isReady();

        virtual bool initilizeBase() = 0;
        virtual bool initilizeSource(QRemoteObjectHost *m_sourceNode) = 0;
        virtual bool initilizeReplica(QRemoteObjectNode *m_replicaNode) = 0;


    public Q_SLOT:
        void setReady(bool ready);
        void setConnectionType(ConnectionType::Type type);

    signals:
        void readyChanged(const bool &ready);
        void connectionTypeChanged(const ConnectionType::Type &type);
        void onStateChange(QRemoteObjectReplica::State);

    private:
        virtual void connectSignals() = 0;

        ConnectionType::Type m_connectionType;
        bool m_ready;
};

#ifndef Q_OS_WASM

#define CONNECT_DPTR(x)     if (this->getConnectionType() == ConnectionType::Type::Local) { \
        QObject::connect(this->d_ptr_internal, &REP_INTERNAL_CLASS::x, this, &REP_PUBLIC_CLASS::x);\
    } else { \
        QObject::connect(this->d_ptr_replica, &REP_REPLICA_CLASS::x, this, &REP_PUBLIC_CLASS::x); \
    };

#define CONNECT_DPTR1(x, y)     if (this->getConnectionType() == ConnectionType::Type::Local) { \
        QObject::connect(this->d_ptr_internal, &REP_INTERNAL_CLASS::x, this, &REP_PUBLIC_CLASS::y);\
    } else { \
        QObject::connect(this->d_ptr_replica, &REP_REPLICA_CLASS::x, this, &REP_PUBLIC_CLASS::y); \
    };

#define CALL_DPTR(x) if (!this->isReady()) return; \
    if (this->getConnectionType() == ConnectionType::Type::Local) { \
        this->d_ptr_internal->x; \
    } else { \
        this->d_ptr_replica->x; \
    }

#define CALL_DPTR_RTN(x, y, ret) if (!this->isReady()) return ret; \
    if (this->getConnectionType() == ConnectionType::Type::Local) \
        return this->d_ptr_internal->x; \
    else { \
        QRemoteObjectPendingReply<y> res = this->d_ptr_replica->x; \
        res.waitForFinished(3000); \
         return res.returnValue(); \
    }

#else 

#define CONNECT_DPTR(x) QObject::connect(this->d_ptr_replica, &REP_REPLICA_CLASS::x, this, &REP_PUBLIC_CLASS::x); 

#define CONNECT_DPTR1(x, y) QObject::connect(this->d_ptr_replica, &REP_REPLICA_CLASS::x, this, &REP_PUBLIC_CLASS::y); 

#define CALL_DPTR(x) this->d_ptr_replica->x

#define CALL_DPTR_RTN(x, y) QRemoteObjectPendingReply<y> res = this->d_ptr_replica->x; \
    res.waitForFinished(); \
    return res.returnValue(); \

#endif

#endif