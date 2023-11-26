; ModuleID = 'test.c'
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.ST = type { i32, double, %struct.RT }
%struct.RT = type { i8, [10 x [20 x i32]], i8 }
%struct.munger_struct = type { i32, i32 }

; Function Attrs: noinline nounwind uwtable
define dso_local ptr @foo(ptr noundef %0) #0 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds %struct.ST, ptr %3, i64 1
  %5 = getelementptr inbounds %struct.ST, ptr %4, i32 0, i32 2
  %6 = getelementptr inbounds %struct.RT, ptr %5, i32 0, i32 1
  %7 = getelementptr inbounds [10 x [20 x i32]], ptr %6, i64 0, i64 5
  %8 = getelementptr inbounds [20 x i32], ptr %7, i64 0, i64 13
  ret ptr %8
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @munge(ptr noundef %0) #0 {
  %2 = alloca ptr, align 8
  store ptr %0, ptr %2, align 8
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds %struct.munger_struct, ptr %3, i64 1
  %5 = getelementptr inbounds %struct.munger_struct, ptr %4, i32 0, i32 0
  %6 = load i32, ptr %5, align 4
  %7 = load ptr, ptr %2, align 8
  %8 = getelementptr inbounds %struct.munger_struct, ptr %7, i64 2
  %9 = getelementptr inbounds %struct.munger_struct, ptr %8, i32 0, i32 1
  %10 = load i32, ptr %9, align 4
  %11 = add nsw i32 %6, %10
  %12 = load ptr, ptr %2, align 8
  %13 = getelementptr inbounds %struct.munger_struct, ptr %12, i64 0
  %14 = getelementptr inbounds %struct.munger_struct, ptr %13, i32 0, i32 0
  store i32 %11, ptr %14, align 4
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca [10 x %struct.ST], align 16
  %3 = alloca ptr, align 8
  %4 = alloca [3 x %struct.munger_struct], align 16
  store i32 0, ptr %1, align 4
  %5 = getelementptr inbounds [10 x %struct.ST], ptr %2, i64 0, i64 0
  %6 = call ptr @foo(ptr noundef %5)
  store ptr %6, ptr %3, align 8
  %7 = getelementptr inbounds [3 x %struct.munger_struct], ptr %4, i64 0, i64 0
  call void @munge(ptr noundef %7)
  ret i32 0
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 16.0.6 (https://github.com/llvm/llvm-project.git 7cbf1a2591520c2491aa35339f227775f4d3adf6)"}
