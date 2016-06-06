#include "stack.h"

Stack::Stack(int length)
{
    Array = new int[length];
    m_length =length;

    m_top = -1;

    //initialize all the value to zero to avoid error in the algorithm
    for(int i = 0; i < m_length; i++)
    {
        Array[i] = -1;
    }
}

Stack::~Stack()
{
    delete [] Array;
}

int Stack::Top()
{
    if(m_top < 0 && m_top >= m_length)
        return -1;
    return Array[m_top];
}

int Stack::Add(int number)
{
    if(In(number))
        return number;
    m_top += 1;
    if(m_top >= m_length)
    {
        m_top-=1;
        return -1;
    }
    Array[m_top] = number;
    return number;
}

int Stack::Pop()
{
    if(m_top < 0)
        return -1;
    int number = Array[m_top];
    Array[m_top] = -1;
    return number;
}

bool Stack::In(int number)
{
    for(int i = 0;i < m_length; i++)
    {
        if(Array[i] == -1)
            break;
        if(Array[i] == number)
            return true;
    }
    return false;
}

int Stack::InIndex(int number)
{
    for(int i=0; i < m_length; i++)
    {
        if(Array[i] == -1)
            break;
        if(Array[i] == number)
            return i;
    }
    return -1;
}

int Stack::Remove(int number)
{
    int pos = InIndex(number);
    if(pos == -1)
        return -1;
    Array[pos] = -1;
    Refresh();
    m_top = -1;
    for(int i=0; i < m_length; i++)
    {
        if(Array[i] != -1)
            m_top = i;
    }
    return number;

}

void Stack::Refresh()
{
    wxLog::SetActiveTarget(new wxLogStderr);
    wxString msg;
    wxLogMessage(msg);
    int holdI;
    for(int i=1; i < m_length; i++)
    {
        if(Array[i] == -1)
            continue;
        else
        {
            if(Array[i-1] == -1)
            {
                holdI = i-1;
                while(true)
                {
                    if(Array[holdI] == -1)
                    {
                        holdI -= 1;
                        //avoid going beyond 0 index
                        if(holdI <= 0)
                        {
                            Array[0] = Array[i];
                            Array[i] = -1;
                            break;
                        }
                    }
                    else
                    {
                        Array[holdI+1] = Array[i];
                        Array[i] = -1;
                        break;
                    }
                }
            }
        }
    }
}

bool Stack::Empty()
{
    for(int i=0; i< m_length; i++)
    {
        if(Array[i] != -1)
            return false;
    }
    return true;
}
