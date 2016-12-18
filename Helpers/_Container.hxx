#pragma once
#include "_Python.hxx"

namespace Python {
    
    template <class T>
    struct const_ref {
        typedef const T &type;
    };

    template <class T>
    struct const_ref<const T> {
        typedef const T &type;
    };

    template <class T>
    struct const_ref<const T &> {
        typedef const T &type;
    };

    template <class T>
    struct const_ref<T *> {
        typedef T *type;
    };

    template <class T>
    struct const_ref<const T *> {
        typedef const T *type;
    };

    enum ReferenceType {
        VALUE, /*! ֵ���� */
        CONST_VALUE, /*! ����ֵ���� */
        REF, /*! ���� */
        CONST_REF, /*! �Գ������� */
        PTR, /*! ָ�� */
        CONST_PTR /*! ����ָ�� */
    };

    /// ����ת��������
    template <class Cont>
    class Container {
    public:

        /// ���캯��
        Container(PyObject *obj, ReferenceType type)
            : m_obj(obj), m_type(type), m_defv(nullptr) {
        }

        /// ����Ĭ��ֵ
        void SetDefault(const Cont &defv) {
            m_defv = const_cast<Cont *>(&defv);
        }

        /// ����Ĭ��ֵ
        void SetDefault(const Cont *defv) {
            m_defv = const_cast<Cont *>(defv);
        }

        /// �������취������ʹ���쳣
        bool ConvertFromPython() {
            if (m_obj && m_obj != Py_None) {
                return DoConvertFromPython();
            }

            return true;
        }

        /// ��������
        virtual ~Container() {}

    protected:

        /// Python -> C++
        virtual bool DoConvertFromPython() = 0;

        /// C++ -> Python
        virtual void DoConvertBackToPython() = 0;

        /// ���Ի�д�� Python
        void TryConvertBackToPython() {
            if (m_obj) {
                if (m_type == REF || (m_type == PTR && m_obj != Py_None)) {
                    DoConvertBackToPython();
                }
            }
        }

    public:

        operator Cont &() {
            return (m_obj) ? m_cont : *m_defv;
        }

        operator Cont *() {
            if (m_obj) {
                return (m_obj == Py_None) ? nullptr : &m_cont;
            }
            else {
                return m_defv;
            }
        }

    protected:

        PyObject *m_obj;
        ReferenceType m_type;

        Cont *m_defv;
        Cont m_cont;
    };
}
