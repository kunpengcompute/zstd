# os_type
%{!?os_type: %define os_type openEuler}

Name: ksal_bpsf%{?version_suffix:_debug}
Version: 1.0.0
Release: %{os_type}
Summary: ksal bpsf compress
License: Commercial

%description
To obtain bpsf rpm

%install
mkdir -p %{buildroot}/usr/lib64
mkdir -p %{buildroot}/usr/include
cp %{_builddir}/*.so %{buildroot}/usr/lib64
cp %{_builddir}/*.h %{buildroot}/usr/include
ln -sf /usr/lib64/libksal_bpsf.so %{buildroot}/usr/lib64/ksal_bpsf.so.1

%files
/usr/lib64/*.so
/usr/lib64/*.so.1
/usr/include/*.h