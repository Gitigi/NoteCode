#ifndef STACK_H
#define STACK_H


class Stack
{
    public:
        Stack(int length = 10);
        virtual ~Stack();

        int Top();
        int Add(int number);
        int Pop();
        bool In(int number);
        bool Empty();
        int Remove(int number);

    protected:

    private:
        int InIndex(int number);
        void Refresh();

        int m_length;
        int *Array;
        int m_top;
};

#endif // STACK_H
