#ifndef ARRAY2D_H
#define ARRAY2D_H
#include <vector>

template<class T>
class array2D
{
private:

public:
    int m_Width;
    int m_Height;
    T* array1D;
    typedef T gridtype;

    array2D(const int& width,const int& height):
        m_Width(width), m_Height(height)
    {
        array1D = new T [width*height];
    }

    array2D(const int& width,const int& height, const T& objectInit):
        m_Width(width), m_Height(height)
    {
        array1D = new T [width*height];
        for (int i=0;i<m_Width*m_Height;i++)
        {
            array1D[i]= objectInit;
        }
    }


    array2D(const array2D& anArray):
        m_Width(anArray.m_Width), m_Height(anArray.m_Height)
    {
        array1D = new gridtype [m_Width*m_Height];
        for (int k=0;k<m_Width*m_Height;k++)
        {
            array1D[k] = anArray.array1D[k];
        }
    }

    array2D operator=(const array2D& anArray)
    {
        if (this == &anArray)
        return *this;
        delete array1D;
        m_Width=anArray.m_Width;
        m_Height=anArray.m_Height;
        array1D = new T [m_Width*m_Height];
        for (int i=0;i<m_Width*m_Height;i++)
        {
            array1D[i]= anArray.array1D[i];
        }
        return *this;
    }
    
    void reset(const T& objectInit)
    {
        for(int i = 0; i<m_Width*m_Height; ++i)
        {
            array1D[i] = objectInit;
        }
        return;
    }

    ~array2D()
    {
        delete array1D;
    }

    T& operator() (const int& x,const int& y)
    {
        return array1D[x*m_Height+y];
    }
};

#endif // ARRAY2D_H
