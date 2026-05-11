#include <gtest/gtest.h>
#include <string>
#include "unique_ptr.h"
#include "tracked.h"

// =============================================================================
//  UniquePtr<T, Deleter> tests
// =============================================================================

TEST(UniquePtrSize, StatelessDeleterNoOverhead) {
    // With [[no_unique_address]], an empty deleter should not increase size
    static_assert(sizeof(UniquePtr<int>) == sizeof(int*),
                  "UniquePtr with default deleter should be the same size as a raw pointer");
    static_assert(sizeof(UniquePtr<double>) == sizeof(double*));
}

TEST(UniquePtrSize, StatefulDeleterIncreasesSize) {
    struct BigDeleter {
        long long state;
        void operator()(int* p) const { delete p; }
    };
    static_assert(sizeof(UniquePtr<int, BigDeleter>) > sizeof(int*),
                  "UniquePtr with a stateful deleter must be larger than a raw pointer");
}

TEST(UniquePtrBasic, DefaultNull) {
    UniquePtr<int> p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(p));
}

TEST(UniquePtrBasic, ConstructFromPointer) {
    UniquePtr<int> p(new int(42));
    EXPECT_NE(p.get(), nullptr);
    EXPECT_TRUE(static_cast<bool>(p));
    EXPECT_EQ(*p, 42);
}

TEST(UniquePtrAccess, Dereference) {
    UniquePtr<int> p(new int(42));
    EXPECT_EQ(*p, 42);
    *p = 100;
    EXPECT_EQ(*p, 100);
}

TEST(UniquePtrAccess, Arrow) {
    UniquePtr<std::string> p(new std::string("hello"));
    EXPECT_EQ(p->size(), 5u);
}

TEST(UniquePtrAccess, ConstGet) {
    const UniquePtr<int> p(new int(42));
    EXPECT_NE(p.get(), nullptr);
    EXPECT_EQ(*p, 42);
}

TEST(UniquePtrMove, MoveConstructor) {
    UniquePtr<int> a(new int(42));
    int* raw = a.get();
    UniquePtr<int> b(std::move(a));
    EXPECT_EQ(a.get(), nullptr);
    EXPECT_EQ(b.get(), raw);
    EXPECT_EQ(*b, 42);
}

TEST(UniquePtrMove, MoveAssignment) {
    UniquePtr<int> a(new int(42));
    UniquePtr<int> b(new int(99));
    int* raw = a.get();
    b = std::move(a);
    EXPECT_EQ(a.get(), nullptr);
    EXPECT_EQ(b.get(), raw);
    EXPECT_EQ(*b, 42);
}

TEST(UniquePtrMove, MoveAssignToEmpty) {
    UniquePtr<int> a(new int(42));
    UniquePtr<int> b;
    b = std::move(a);
    EXPECT_EQ(a.get(), nullptr);
    EXPECT_EQ(*b, 42);
}

TEST(UniquePtrModifiers, Release) {
    UniquePtr<int> p(new int(42));
    int* raw = p.release();
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(*raw, 42);
    delete raw;  // manual cleanup since ownership released
}

TEST(UniquePtrModifiers, ResetToNew) {
    UniquePtr<int> p(new int(42));
    p.reset(new int(99));
    EXPECT_EQ(*p, 99);
}

TEST(UniquePtrModifiers, ResetToNull) {
    UniquePtr<int> p(new int(42));
    p.reset();
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(p));
}

TEST(UniquePtrModifiers, Swap) {
    UniquePtr<int> a(new int(1));
    UniquePtr<int> b(new int(2));
    int* ra = a.get();
    int* rb = b.get();
    a.swap(b);
    EXPECT_EQ(a.get(), rb);
    EXPECT_EQ(b.get(), ra);
    EXPECT_EQ(*a, 2);
    EXPECT_EQ(*b, 1);
}

TEST(UniquePtrDeleter, CustomDeleter) {
    bool deleted = false;
    auto deleter = [&deleted](int* p) {
        deleted = true;
        delete p;
    };
    {
        UniquePtr<int, decltype(deleter)> p(new int(42), deleter);
        EXPECT_EQ(*p, 42);
    }
    EXPECT_TRUE(deleted);
}

TEST(UniquePtrDeleter, GetDeleter) {
    bool deleted = false;
    auto deleter = [&deleted](int* p) {
        deleted = true;
        delete p;
    };
    UniquePtr<int, decltype(deleter)> p(new int(42), deleter);
    p.get_deleter()(p.release());
    EXPECT_TRUE(deleted);
}

TEST(UniquePtrDeleter, ResetUsesDeleter) {
    int delete_count = 0;
    auto deleter = [&delete_count](int* p) {
        ++delete_count;
        delete p;
    };
    {
        UniquePtr<int, decltype(deleter)> p(new int(1), deleter);
        p.reset(new int(2));
        EXPECT_EQ(delete_count, 1);
    }
    EXPECT_EQ(delete_count, 2);
}

TEST(MakeUnique, Basic) {
    auto p = make_unique<int>(42);
    EXPECT_EQ(*p, 42);
}

TEST(MakeUnique, WithString) {
    auto p = make_unique<std::string>(5, 'x');
    EXPECT_EQ(*p, "xxxxx");
}

TEST(MakeUnique, DefaultConstructed) {
    auto p = make_unique<int>();
    EXPECT_NE(p.get(), nullptr);
}

TEST_F(TrackedTest, UniquePtrDestructor) {
    {
        UniquePtr<Tracked> p(new Tracked(10));
        EXPECT_EQ(p->value, 10);
    }
}

TEST_F(TrackedTest, UniquePtrResetDestroys) {
    {
        UniquePtr<Tracked> p(new Tracked(10));
        p.reset();
        EXPECT_EQ(alive_count, 0);
    }
}

TEST_F(TrackedTest, UniquePtrMoveNoLeak) {
    {
        UniquePtr<Tracked> a(new Tracked(5));
        UniquePtr<Tracked> b(std::move(a));
        EXPECT_EQ(b->value, 5);
    }
}

TEST_F(TrackedTest, UniquePtrMoveAssignNoLeak) {
    {
        UniquePtr<Tracked> a(new Tracked(1));
        UniquePtr<Tracked> b(new Tracked(2));
        b = std::move(a);
    }
}

TEST_F(TrackedTest, MakeUniqueNoLeak) {
    {
        auto p = make_unique<Tracked>(42);
        EXPECT_EQ(p->value, 42);
    }
}

// =============================================================================
//  UniquePtr<T[]> array specialization tests
// =============================================================================

TEST(UniquePtrArray, DefaultNull) {
    UniquePtr<int[]> p;
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_FALSE(static_cast<bool>(p));
}

TEST(UniquePtrArray, ConstructFromPointer) {
    UniquePtr<int[]> p(new int[3]{10, 20, 30});
    EXPECT_NE(p.get(), nullptr);
    EXPECT_TRUE(static_cast<bool>(p));
}

TEST(UniquePtrArray, Subscript) {
    UniquePtr<int[]> p(new int[3]{10, 20, 30});
    EXPECT_EQ(p[0], 10);
    EXPECT_EQ(p[1], 20);
    EXPECT_EQ(p[2], 30);
}

TEST(UniquePtrArray, SubscriptWrite) {
    UniquePtr<int[]> p(new int[3]{});
    p[0] = 100;
    p[1] = 200;
    p[2] = 300;
    EXPECT_EQ(p[0], 100);
    EXPECT_EQ(p[1], 200);
    EXPECT_EQ(p[2], 300);
}

TEST(UniquePtrArray, ConstSubscript) {
    const UniquePtr<int[]> p(new int[3]{10, 20, 30});
    EXPECT_EQ(p[0], 10);
    EXPECT_EQ(p[1], 20);
    EXPECT_EQ(p[2], 30);
}

TEST(UniquePtrArray, MoveConstructor) {
    UniquePtr<int[]> a(new int[2]{1, 2});
    int* raw = a.get();
    UniquePtr<int[]> b(std::move(a));
    EXPECT_EQ(a.get(), nullptr);
    EXPECT_EQ(b.get(), raw);
    EXPECT_EQ(b[0], 1);
    EXPECT_EQ(b[1], 2);
}

TEST(UniquePtrArray, MoveAssignment) {
    UniquePtr<int[]> a(new int[2]{1, 2});
    UniquePtr<int[]> b(new int[3]{3, 4, 5});
    b = std::move(a);
    EXPECT_EQ(a.get(), nullptr);
    EXPECT_EQ(b[0], 1);
}

TEST(UniquePtrArray, Release) {
    UniquePtr<int[]> p(new int[2]{10, 20});
    int* raw = p.release();
    EXPECT_EQ(p.get(), nullptr);
    EXPECT_EQ(raw[0], 10);
    delete[] raw;
}

TEST(UniquePtrArray, Reset) {
    UniquePtr<int[]> p(new int[2]{1, 2});
    p.reset(new int[3]{3, 4, 5});
    EXPECT_EQ(p[0], 3);
    EXPECT_EQ(p[2], 5);
}

TEST(UniquePtrArray, ResetToNull) {
    UniquePtr<int[]> p(new int[2]{1, 2});
    p.reset();
    EXPECT_EQ(p.get(), nullptr);
}

TEST(UniquePtrArray, Swap) {
    UniquePtr<int[]> a(new int[2]{1, 2});
    UniquePtr<int[]> b(new int[2]{3, 4});
    int* ra = a.get();
    int* rb = b.get();
    a.swap(b);
    EXPECT_EQ(a.get(), rb);
    EXPECT_EQ(b.get(), ra);
}

TEST(UniquePtrArray, CustomDeleter) {
    bool deleted = false;
    auto deleter = [&deleted](int* p) {
        deleted = true;
        delete[] p;
    };
    {
        UniquePtr<int[], decltype(deleter)> p(new int[3]{1, 2, 3}, deleter);
        EXPECT_EQ(p[1], 2);
    }
    EXPECT_TRUE(deleted);
}

TEST_F(TrackedTest, UniquePtrArrayDestructor) {
    {
        UniquePtr<Tracked[]> p(new Tracked[3]);
        EXPECT_EQ(alive_count, 3);
    }
}

TEST_F(TrackedTest, UniquePtrArrayMoveNoLeak) {
    {
        UniquePtr<Tracked[]> a(new Tracked[3]);
        UniquePtr<Tracked[]> b(std::move(a));
        EXPECT_EQ(alive_count, 3);
    }
}

TEST_F(TrackedTest, UniquePtrArrayResetNoLeak) {
    {
        UniquePtr<Tracked[]> p(new Tracked[3]);
        p.reset();
        EXPECT_EQ(alive_count, 0);
    }
}