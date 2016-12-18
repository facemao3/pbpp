/***************************************************************
 * Name:      _PySelf.hxx
 * Purpose:   ��������� PyObject ָ�루ģ�� self ָ�룩
 * Author:    Wang Xiaoning (vanxining@139.com)
 * Created:   2014-11-04
 **************************************************************/
#pragma once
#include "_Python.hxx"

/// ��������� PyObject ָ��
class _PySelf {
public:
    _PySelf();
    ~_PySelf();

    void SetSelf(PyObject *self, bool clone = false);
    /// return: new reference
    PyObject *GetSelf() const;

    bool IsCloned() const {
        return m_cloned;
    }

protected:

    PyObject *m_self;
    bool m_cloned;
};
