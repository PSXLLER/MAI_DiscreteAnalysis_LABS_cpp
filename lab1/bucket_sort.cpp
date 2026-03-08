#include <cstdio>

const int INITIAL_CAPACITY = 8;
const int RESIZE_FACTOR = 2;

const int BUCKETS_COUNT = 100000;

const double RANGE_MIN = -100.0;
const double RANGE_MAX = 100.0;

const int ZERO = 0;
const int ONE = 1;

const int TWO_VALUES = 2;
const int PRECISION = 6;

struct TKeyValuePair {
    double Key;
    unsigned long long Value;
};

class TVector {
public:
    TVector() {
        Capacity = INITIAL_CAPACITY;
        Size = ZERO;
        Data = new TKeyValuePair[Capacity];
    }

    ~TVector() {
        delete[] Data;
    }

    void PushBack(const TKeyValuePair& element) {
        if (Size == Capacity) {
            Resize();
        }

        Data[Size] = element;
        Size = Size + ONE;
    }

    TKeyValuePair& operator[](int index) {
        return Data[index];
    }

    int GetSize() const {
        return Size;
    }

private:
    void Resize() {

        Capacity = Capacity * RESIZE_FACTOR;

        TKeyValuePair* newData =
            new TKeyValuePair[Capacity];

        for (int i = ZERO; i < Size; i = i + ONE) {
            newData[i] = Data[i];
        }

        delete[] Data;
        Data = newData;
    }

    int Size;
    int Capacity;
    TKeyValuePair* Data;
};

void InsertionSort(TVector& bucket) {

    int n = bucket.GetSize();

    for (int i = ONE; i < n; i = i + ONE) {

        TKeyValuePair key = bucket[i];

        int j = i - ONE;

        while (j >= ZERO && bucket[j].Key > key.Key) {
            bucket[j + ONE] = bucket[j];
            j = j - ONE;
        }

        bucket[j + ONE] = key;
    }
}

void BucketSort(TVector& input) {

    int n = input.GetSize();

    if (n == ZERO) {
        return;
    }

    TVector* buckets =
        new TVector[BUCKETS_COUNT];

    double bucketWidth =
        (RANGE_MAX - RANGE_MIN)
        / BUCKETS_COUNT;

    for (int i = ZERO; i < n; i = i + ONE) {

        int index =
            static_cast<int>(
                (input[i].Key - RANGE_MIN)
                / bucketWidth
            );

        if (index < ZERO) {
            index = ZERO;
        }

        if (index >= BUCKETS_COUNT) {
            index = BUCKETS_COUNT - ONE;
        }

        buckets[index].PushBack(input[i]);
    }

    int currentIndex = ZERO;

    for (int i = ZERO; i < BUCKETS_COUNT; i = i + ONE) {

        if (buckets[i].GetSize() > ONE) {
            InsertionSort(buckets[i]);
        }

        int bucketSize = buckets[i].GetSize();

        for (int j = ZERO; j < bucketSize; j = j + ONE) {

            input[currentIndex] = buckets[i][j];
            currentIndex = currentIndex + ONE;
        }
    }

    delete[] buckets;
}

int main() {

    TVector input;
    TKeyValuePair temp;

    while (scanf("%lf%llu", &temp.Key, &temp.Value) == TWO_VALUES) {
        input.PushBack(temp);
    }

    BucketSort(input);

    int n = input.GetSize();

    for (int i = ZERO; i < n; i = i + ONE) {

       printf("%.*f\t%llu\n",
       PRECISION,
       input[i].Key,
       input[i].Value);
    }

    return ZERO;
}