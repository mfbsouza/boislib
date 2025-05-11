const std = @import("std");
const zcc = @import("compile_commands");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // --- configure the C library ---
    const boislib = b.addStaticLibrary(.{
        .name = "boislib",
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    boislib.addCSourceFiles(.{
        .flags = &.{},
        .files = &.{
            "src/memory/allocator.c",
            "src/queue/circular_queue.c",
        },
    });
    boislib.installHeader(b.path("src/memory/allocator.h"), "boislib/allocator.h");
    boislib.installHeader(b.path("src/queue/circular_queue.h"), "boislib/circular_queue.h");

    b.installArtifact(boislib);
    const boislib_step = b.step("boislib", "Build boislib static library");
    const install_boislib = b.addInstallArtifact(boislib, .{});
    boislib_step.dependOn(&install_boislib.step);
    
    // --- configure the test executable ---
    const tests = b.addExecutable(.{
        .name = "boislib_tests",
        .target = target,
        .optimize = optimize,
    });
    const googletest_dep = b.dependency("googletest", .{
        .target = target,
        .optimize = optimize,
    });
    tests.linkLibCpp();
    tests.linkLibrary(googletest_dep.artifact("gtest"));
    tests.linkLibrary(googletest_dep.artifact("gtest_main"));
    tests.linkLibrary(boislib);
    tests.addCSourceFiles(.{
        .flags = &.{},
        .files = &.{
            "tests/allocator_tests.cpp",
            "tests/circular_queue_tests.cpp",
        },
    });

    b.installArtifact(tests);
    const tests_step = b.step("tests", "Build boislib test executable");
    const install_tests = b.addInstallArtifact(tests, .{});
    tests_step.dependOn(&install_tests.step);

    // --- step for generating compile commands ---
    var targets = std.ArrayList(*std.Build.Step.Compile).init(b.allocator);
    targets.append(boislib) catch @panic("OOM");
    targets.append(tests) catch @panic("OOM");
    zcc.createStep(b, "cdb", targets.toOwnedSlice() catch @panic("OOM"));
}
